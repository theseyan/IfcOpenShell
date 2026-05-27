// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/pset_template.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"

#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

struct ifcopenshell_pset_template_t {
    std::string normalized_schema;
    const IfcParse::schema_definition* schema = nullptr;
    std::unique_ptr<IfcParse::IfcFile> templates_file;
    std::vector<const IfcParse::IfcFile*> template_files;
};

namespace {

inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

std::mutex g_template_mutex;
std::map<std::string, std::unique_ptr<ifcopenshell_pset_template_t>> g_templates;
std::string g_template_dir;

std::string normalize_schema(const std::string& v) {
    if (v.rfind("IFC4X3", 0) == 0) return "IFC4X3";
    if (v.rfind("IFC4", 0) == 0)   return "IFC4";
    if (v.rfind("IFC2X3", 0) == 0) return "IFC2X3";
    return std::string();
}

std::string template_filename(const std::string& schema) {
    if (schema == "IFC2X3") return "Pset_IFC2X3.ifc";
    if (schema == "IFC4")   return "Pset_IFC4_ADD2.ifc";
    if (schema == "IFC4X3") return "Pset_IFC4X3.ifc";
    return std::string();
}

inline IfcUtil::IfcBaseEntity* as_entity(IfcUtil::IfcBaseClass* e) {
    return e ? dynamic_cast<IfcUtil::IfcBaseEntity*>(e) : nullptr;
}

std::string read_string(IfcUtil::IfcBaseClass* e, const char* name) {
    auto* be = as_entity(e);
    if (!be) return std::string();
    try {
        auto av = be->get(name);
        if (av.isNull()) return std::string();
        return (std::string)av;
    } catch (...) {
        return std::string();
    }
}

bool decl_is(const IfcParse::declaration* decl, const char* name) {
    if (!decl) return false;
    return decl->is(name);
}

void apply_ifc4_patch(IfcParse::IfcFile* f) {
    if (!f) return;
    auto entities = f->instances_by_type("IfcPropertySetTemplate");
    if (!entities) return;
    for (auto it = entities->begin(); it != entities->end(); ++it) {
        auto* be = as_entity(*it);
        if (!be) continue;
        try {
            auto av = be->get("TemplateType");
            if (av.isNull()) continue;
            std::string current = (std::string)av;
            if (current == "QTO_OCCURRENCEDRIVEN") {
                (*it)->set_attribute_value(std::string("TemplateType"), std::string("QTO_TYPEDRIVENOVERRIDE"));
            }
        } catch (...) {
            continue;
        }
    }
}

ifcopenshell_pset_template_t* get_or_load_locked(const std::string& schema_id) {
    std::string normalized = normalize_schema(schema_id);
    if (normalized.empty()) {
        set_error("Unexpected schema version: " + schema_id);
        return nullptr;
    }
    auto it = g_templates.find(normalized);
    if (it != g_templates.end()) return it->second.get();

    if (g_template_dir.empty()) {
        set_error("Template directory not set; call ifcopenshell_util_pset_set_template_dir() first");
        return nullptr;
    }

    std::string fn = template_filename(normalized);
    if (fn.empty()) {
        set_error("Unsupported schema for templates: " + normalized);
        return nullptr;
    }
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    std::string path = g_template_dir;
    if (!path.empty() && path.back() != '/' && path.back() != '\\') path += sep;
    path += fn;

    std::unique_ptr<IfcParse::IfcFile> tf;
    try {
        tf.reset(new IfcParse::IfcFile(path));
    } catch (const std::exception& e) {
        set_error(std::string("Failed to load template file '") + path + "': " + e.what());
        return nullptr;
    }
    if (!tf || !tf->good()) {
        set_error("Failed to load template file: " + path);
        return nullptr;
    }

    if (normalized == "IFC4") {
        apply_ifc4_patch(tf.get());
    }

    auto cache = std::make_unique<ifcopenshell_pset_template_t>();
    cache->normalized_schema = normalized;
    try {
        cache->schema = IfcParse::schema_by_name(normalized);
    } catch (...) {
        cache->schema = nullptr;
    }
    cache->templates_file = std::move(tf);
    cache->template_files.push_back(cache->templates_file.get());

    auto* raw = cache.get();
    g_templates.emplace(normalized, std::move(cache));
    return raw;
}

bool startswith(const std::string& s, const char* prefix) {
    size_t n = std::strlen(prefix);
    return s.size() >= n && std::memcmp(s.data(), prefix, n) == 0;
}

std::string lower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

bool is_applicable(const IfcParse::schema_definition* schema,
                   const IfcParse::entity* entity,
                   const std::string& applicables,
                   const std::string& predefined_type,
                   const std::string& template_type)
{
    if (!entity) return false;
    static const std::regex re(R"((\w+)(\[\w+\])*/*(\w+)*(\[\w+\])*)");
    std::string remaining = applicables;
    while (!remaining.empty()) {
        std::string item;
        auto comma = remaining.find(',');
        if (comma == std::string::npos) {
            item = remaining;
            remaining.clear();
        } else {
            item = remaining.substr(0, comma);
            remaining = remaining.substr(comma + 1);
        }
        std::smatch match;
        if (!std::regex_search(item, match, re)) continue;
        std::string applicable_class = match[1].str();
        std::string matched_type = match[3].matched ? match[3].str() : std::string();

        if (!matched_type.empty() && predefined_type.empty()) continue;
        if (!matched_type.empty() && lower(predefined_type) != lower(matched_type)) continue;

        if (decl_is(entity, applicable_class.c_str())) return true;

        if (template_type.find("TYPE") != std::string::npos && entity->is("IfcTypeObject")) {
            std::vector<std::string> types;
            if (schema) {
                std::string occ;
                try {
                    auto* d = schema->declaration_by_name(applicable_class + "Type");
                    if (d) occ = d->name();
                } catch (...) {}
                if (occ.empty() && applicable_class.size() > 3) {
                    try {
                        auto* d = schema->declaration_by_name("IfcType" + applicable_class.substr(3));
                        if (d) occ = d->name();
                    } catch (...) {}
                }
                if (!occ.empty()) types.push_back(occ);
            }
            for (const auto& t : types) {
                if (decl_is(entity, t.c_str())) return true;
            }
        }
    }
    return false;
}

bool template_passes_filters(IfcUtil::IfcBaseClass* prop_set, bool pset_only, bool qto_only,
                             std::string& out_template_type) {
    out_template_type = read_string(prop_set, "TemplateType");
    if (pset_only && !out_template_type.empty() && startswith(out_template_type, "QTO_")) return false;
    if (qto_only && !out_template_type.empty() && startswith(out_template_type, "PSET_")) return false;
    return true;
}

}  // namespace

namespace ifcapi {
namespace bindings {

void pset_template_set_template_dir(const std::string& dir) {
    std::lock_guard<std::mutex> lk(g_template_mutex);
    g_template_dir = dir;
}

ifcopenshell_pset_template_t* pset_template_get_template(const std::string& schema_identifier) {
    if (schema_identifier.empty()) { set_error("schema_identifier is empty"); return nullptr; }
    std::lock_guard<std::mutex> lk(g_template_mutex);
    return get_or_load_locked(schema_identifier);
}

ifcopenshell_pset_template_t* pset_template_create_from_files(
    const std::string& schema_identifier,
    const std::vector<const IfcParse::IfcFile*>& template_files)
{
    if (schema_identifier.empty()) { set_error("schema_identifier is empty"); return nullptr; }
    if (template_files.empty()) { set_error("template_files is empty"); return nullptr; }

    std::string normalized = normalize_schema(schema_identifier);
    if (normalized.empty()) {
        set_error("Unexpected schema version: " + schema_identifier);
        return nullptr;
    }

    auto cache = std::make_unique<ifcopenshell_pset_template_t>();
    cache->normalized_schema = normalized;
    try {
        cache->schema = IfcParse::schema_by_name(normalized);
    } catch (...) {
        cache->schema = nullptr;
    }
    for (const auto* file : template_files) {
        if (file) cache->template_files.push_back(file);
    }
    if (cache->template_files.empty()) {
        set_error("template_files contains no valid files");
        return nullptr;
    }
    return cache.release();
}

void pset_template_free(ifcopenshell_pset_template_t* pqt)
{
    delete pqt;
}

IfcUtil::IfcBaseClass* pset_template_get_by_name(ifcopenshell_pset_template_t* pqt, const std::string& name)
{
    if (!pqt || name.empty()) return nullptr;
    std::lock_guard<std::mutex> lk(g_template_mutex);
    for (const auto* f_const : pqt->template_files) {
        auto* f = const_cast<IfcParse::IfcFile*>(f_const);
        if (!f) continue;
        auto entities = f->instances_by_type("IfcPropertySetTemplate");
        if (!entities) continue;
        for (auto it = entities->begin(); it != entities->end(); ++it) {
            if (read_string(*it, "Name") == name) return *it;
        }
    }
    return nullptr;
}

bool pset_template_is_templated(ifcopenshell_pset_template_t* pqt, const std::string& name)
{
    if (!pqt || name.empty()) return false;
    std::lock_guard<std::mutex> lk(g_template_mutex);
    for (const auto* f_const : pqt->template_files) {
        auto* f = const_cast<IfcParse::IfcFile*>(f_const);
        if (!f) continue;
        auto entities = f->instances_by_type("IfcPropertySetTemplate");
        if (!entities) continue;
        for (auto it = entities->begin(); it != entities->end(); ++it) {
            if (read_string(*it, "Name") == name) return true;
        }
    }
    return false;
}

std::vector<IfcUtil::IfcBaseClass*> pset_template_get_applicable(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema_name)
{
    if (!pqt) return {};
    std::lock_guard<std::mutex> lk(g_template_mutex);

    std::string ifc_class_s = ifc_class ? ifc_class : "";
    std::string predefined_s = predefined_type ? predefined_type : "";

    bool any_class = ifc_class_s.empty();

    const IfcParse::schema_definition* effective_schema = nullptr;
    const IfcParse::entity* entity_decl = nullptr;
    if (!any_class) {
        try {
            effective_schema = pqt->schema;
            if (!effective_schema) effective_schema = IfcParse::schema_by_name(pqt->normalized_schema);
        } catch (...) { effective_schema = nullptr; }
        if (!effective_schema) {
            set_error("Cannot resolve schema for ifc_class lookup");
            return {};
        }
        try {
            auto* decl = effective_schema->declaration_by_name(ifc_class_s);
            entity_decl = decl ? decl->as_entity() : nullptr;
        } catch (...) { entity_decl = nullptr; }
        if (!entity_decl) {
            set_error("ifc_class not an entity: " + ifc_class_s);
            return {};
        }
    }

    std::vector<IfcUtil::IfcBaseClass*> result;
    for (const auto* f_const : pqt->template_files) {
        auto* f = const_cast<IfcParse::IfcFile*>(f_const);
        if (!f) continue;
        auto entities = f->instances_by_type("IfcPropertySetTemplate");
        if (!entities) continue;
        for (auto it = entities->begin(); it != entities->end(); ++it) {
            std::string template_type;
            if (!template_passes_filters(*it, pset_only, qto_only, template_type)) continue;
            if (any_class) {
                result.push_back(*it);
            } else {
                std::string applicables = read_string(*it, "ApplicableEntity");
                if (applicables.empty()) applicables = "IfcRoot";
                if (is_applicable(effective_schema, entity_decl, applicables, predefined_s, template_type)) {
                    result.push_back(*it);
                }
            }
        }
    }
    (void)schema_name;
    return result;
}

std::vector<std::string> pset_template_get_applicable_names(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema_name)
{
    auto applicable = pset_template_get_applicable(pqt, ifc_class, predefined_type, pset_only, qto_only, schema_name);
    std::vector<std::string> names;
    names.reserve(applicable.size());
    for (auto* item : applicable) {
        names.push_back(read_string(item, "Name"));
    }
    return names;
}

std::string pset_template_pset_type(IfcUtil::IfcBaseClass* e) {
    if (!e) return "";
    std::string template_type = read_string(e, "TemplateType");
    if (!template_type.empty()) {
        if (startswith(template_type, "PSET_")) return "PSET";
        if (startswith(template_type, "QTO_"))  return "QTO";
    }
    auto* be = as_entity(e);
    if (!be) return nullptr;
    bool seen_pset = false, seen_qto = false;
    try {
        auto av = be->get("HasPropertyTemplates");
        if (av.isNull()) return nullptr;
        auto agg = (boost::shared_ptr<aggregate_of_instance>)av;
        if (!agg) return nullptr;
        for (size_t i = 0; i < agg->size(); ++i) {
            auto* prop = (*agg)[i];
            std::string ptt = read_string(prop, "TemplateType");
            if (ptt.empty()) continue;
            if (startswith(ptt, "P_")) seen_pset = true;
            else seen_qto = true;
        }
    } catch (...) {
        return "";
    }
    if (seen_pset && !seen_qto) return "PSET";
    if (seen_qto && !seen_pset) return "QTO";
    return "";
}

} // namespace bindings
} // namespace ifcapi
