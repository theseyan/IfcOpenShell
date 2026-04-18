// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

namespace {

IfcUtil::IfcBaseClass* get_entity(const ifcopenshell_ifc_instance_t* instance) {
    return instance ? instance->ptr : nullptr;
}

IfcParse::IfcFile* as_file(const ifcopenshell_ifc_file_t* file) {
    return file->ptr;
}

bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

int32_t id_of(IfcUtil::IfcBaseClass* e) {
    return e ? static_cast<int32_t>(e->id()) : 0;
}

IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    auto* d = be->declaration().as_entity();
    if (!d) return nullptr;
    int idx = d->attribute_index(attr);
    if (idx < 0) return nullptr;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return nullptr;
        return (IfcUtil::IfcBaseClass*)val;
    } catch (...) {
        return nullptr;
    }
}

std::string read_string(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return {};
    auto* d = be->declaration().as_entity();
    if (!d) return {};
    int idx = d->attribute_index(attr);
    if (idx < 0) return {};
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return {};
        return (std::string)val;
    } catch (...) {
        return {};
    }
}

std::vector<IfcUtil::IfcBaseClass*> read_ref_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return out;
    auto* d = be->declaration().as_entity();
    if (!d) return out;
    int idx = d->attribute_index(attr);
    if (idx < 0) return out;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return out;
        auto agg = (aggregate_of_instance::ptr)val;
        if (!agg) return out;
        for (auto& item : *agg) out.push_back(item);
    } catch (...) {}
    return out;
}

// Read a numeric attribute; returns default_val when null or missing.
double read_double(IfcUtil::IfcBaseClass* e, const char* attr, double default_val = 0.0) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return default_val;
    auto* d = be->declaration().as_entity();
    if (!d) return default_val;
    int idx = d->attribute_index(attr);
    if (idx < 0) return default_val;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return default_val;
        return (double)val;
    } catch (...) {
        return default_val;
    }
}

bool str_eq_opt(const std::string& s, const char* want) {
    if (!want || !*want) return true;
    return s == want;
}

ifcopenshell_ifc_instance_t** alloc_id_handles(IfcParse::IfcFile* f, const std::vector<int32_t>& ids, uint32_t* out_count) {
    if (out_count) *out_count = static_cast<uint32_t>(ids.size());
    if (ids.empty()) return nullptr;
    auto* buf = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(ids.size() * sizeof(ifcopenshell_ifc_instance_t*)));
    if (!buf) {
        if (out_count) *out_count = 0;
        return nullptr;
    }
    for (size_t i = 0; i < ids.size(); ++i) {
        auto* e = f ? f->instance_by_id(ids[i]) : nullptr;
        buf[i] = ifcopenshell::capi::wrap_instance(e);
    }
    return buf;
}

void collect_base_items(IfcUtil::IfcBaseClass* rep, std::vector<int32_t>& out, int depth = 0) {
    if (!rep || depth > 64) return;
    auto items = read_ref_list(rep, "Items");
    std::vector<IfcUtil::IfcBaseClass*> queue(items.begin(), items.end());
    int guard = 0;
    while (!queue.empty() && guard++ < 100000) {
        auto* item = queue.back();
        queue.pop_back();
        if (!item) continue;
        if (is_a(item, "IfcMappedItem")) {
            auto* src = read_ref(item, "MappingSource");
            if (!src) continue;
            auto* mapped = read_ref(src, "MappedRepresentation");
            if (!mapped) continue;
            collect_base_items(mapped, out, depth + 1);
        } else if (is_a(item, "IfcBooleanResult")) {
            auto* first = read_ref(item, "FirstOperand");
            auto* second = read_ref(item, "SecondOperand");
            if (first) queue.push_back(first);
            if (second) queue.push_back(second);
        } else {
            out.push_back(id_of(item));
        }
    }
}

}  // namespace

extern "C" {

ifcopenshell_ifc_instance_t* ifcopenshell_representation_get_context(
    const ifcopenshell_ifc_file_t* file,
    const char* context_type,
    const char* subcontext,
    const char* target_view)
{
    auto* f = as_file(file);
    if (!f) return 0;

    bool use_sub = (subcontext && *subcontext) || (target_view && *target_view);
    auto insts = f->instances_by_type(
        std::string(use_sub ? "IfcGeometricRepresentationSubContext"
                            : "IfcGeometricRepresentationContext"));
    if (!insts) return 0;

    for (auto it = insts->begin(); it != insts->end(); ++it) {
        auto* e = *it;
        // If we asked only for a base context, exclude subcontexts.
        if (!use_sub && is_a(e, "IfcGeometricRepresentationSubContext")) continue;

        if (context_type && *context_type) {
            if (read_string(e, "ContextType") != context_type) continue;
        }
        if (subcontext && *subcontext) {
            if (read_string(e, "ContextIdentifier") != subcontext) continue;
        }
        if (target_view && *target_view) {
            if (read_string(e, "TargetView") != target_view) continue;
        }
        return ifcopenshell::capi::wrap_instance(e);
    }
    return nullptr;
}

ifcopenshell_ifc_instance_t* ifcopenshell_representation_resolve(const ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* rep) {
    auto* e = rep ? rep->ptr : nullptr;
    if (!e) return nullptr;

    // Tekla 2023 workaround: a representation with a single IfcMappedItem whose
    // mapping source points to another representation is unwrapped.
    for (int guard = 0; guard < 64; ++guard) {
        auto items = read_ref_list(e, "Items");
        if (items.size() != 1 || !is_a(items[0], "IfcMappedItem")) break;
        auto* src = read_ref(items[0], "MappingSource");
        if (!src) break;
        auto* mapped = read_ref(src, "MappedRepresentation");
        if (!mapped) break;
        e = mapped;
    }
    return ifcopenshell::capi::wrap_instance(e);
}

ifcopenshell_ifc_instance_t* ifcopenshell_representation_get_product_representation(
    const ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* element,
    ifcopenshell_ifc_instance_t* context,              // 0 = match by string attrs instead
    const char* context_type,
    const char* subcontext,
    const char* target_view)
{
    auto* e = element ? element->ptr : nullptr;
    if (!e) return nullptr;

    std::vector<IfcUtil::IfcBaseClass*> reps;
    if (is_a(e, "IfcProduct")) {
        auto* rep = read_ref(e, "Representation");
        if (rep) {
            reps = read_ref_list(rep, "Representations");
        }
    } else if (is_a(e, "IfcTypeProduct")) {
        auto maps = read_ref_list(e, "RepresentationMaps");
        for (auto* m : maps) {
            auto* r = read_ref(m, "MappedRepresentation");
            if (r) reps.push_back(r);
        }
    }

    auto* context_e = context ? context->ptr : nullptr;
    for (auto* r : reps) {
        auto* ctx = read_ref(r, "ContextOfItems");
        if (!ctx) continue;

        if (context_e != nullptr) {
            if (ctx == context_e) return ifcopenshell::capi::wrap_instance(r);
            continue;
        }
        if (target_view && *target_view) {
            if (!is_a(ctx, "IfcGeometricRepresentationSubContext")) continue;
            if (read_string(ctx, "TargetView") != target_view) continue;
            if (read_string(ctx, "ContextIdentifier") != (subcontext ? subcontext : "")) continue;
            if (read_string(ctx, "ContextType") != (context_type ? context_type : "")) continue;
            return ifcopenshell::capi::wrap_instance(r);
        }
        if (subcontext && *subcontext) {
            if (!is_a(ctx, "IfcGeometricRepresentationSubContext")) continue;
            if (read_string(ctx, "ContextIdentifier") != subcontext) continue;
            if (!str_eq_opt(read_string(ctx, "ContextType"), context_type)) continue;
            return ifcopenshell::capi::wrap_instance(r);
        }
        if (!str_eq_opt(read_string(ctx, "ContextType"), context_type)) continue;
        return ifcopenshell::capi::wrap_instance(r);
    }
    return nullptr;
}

ifcopenshell_ifc_instance_t** ifcopenshell_representation_resolve_base_items(
    const ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* rep, uint32_t* out_count)
{
    if (out_count) *out_count = 0;
    auto* e = rep ? rep->ptr : nullptr;
    if (!e) return nullptr;
    std::vector<int32_t> result;
    collect_base_items(e, result);
    return alloc_id_handles(as_file(file), result, out_count);
}

ifcopenshell_ifc_instance_t** ifcopenshell_representation_get_prioritised_contexts(
    const ifcopenshell_ifc_file_t* file, uint32_t* out_count)
{
    if (out_count) *out_count = 0;
    auto* f = as_file(file);
    if (!f) return nullptr;

    static const std::vector<std::string> TYPE_PRIORITY = {"Model", "Plan", "Annotation"};
    static const std::vector<std::string> IDENTIFIER_PRIORITY = {
        "Body", "Body-FallBack", "Facetation", "FootPrint", "Profile", "Surface",
        "Reference", "Axis", "Clearance", "Box", "Lighting", "Annotation", "CoG",
    };
    static const std::vector<std::string> TARGET_VIEW_PRIORITY = {
        "MODEL_VIEW", "PLAN_VIEW", "REFLECTED_PLAN_VIEW", "ELEVATION_VIEW",
        "SECTION_VIEW", "GRAPH_VIEW", "SKETCH_VIEW", "USERDEFINED", "NOTDEFINED",
    };

    auto score_in = [](const std::vector<std::string>& table, const std::string& val) -> int {
        if (val.empty()) return 0;
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i] == val) return static_cast<int>(table.size() - i);
        }
        return 0;
    };

    struct Entry {
        int32_t id;
        int type_p;
        int id_p;
        int view_p;
        double scale;
    };
    std::vector<Entry> entries;

    auto insts = f->instances_by_type(std::string("IfcGeometricRepresentationContext"));
    if (insts) {
        for (auto it = insts->begin(); it != insts->end(); ++it) {
            auto* c = *it;
            Entry e;
            e.id = id_of(c);
            e.type_p = score_in(TYPE_PRIORITY, read_string(c, "ContextType"));
            e.id_p = score_in(IDENTIFIER_PRIORITY, read_string(c, "ContextIdentifier"));
            e.view_p = is_a(c, "IfcGeometricRepresentationSubContext")
                           ? score_in(TARGET_VIEW_PRIORITY, read_string(c, "TargetView"))
                           : 0;
            e.scale = is_a(c, "IfcGeometricRepresentationSubContext")
                          ? read_double(c, "TargetScale", 0.0)
                          : 0.0;
            entries.push_back(e);
        }
    }

    // Sort by tuple descending (matching Python's reverse=True on positive priorities).
    std::stable_sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        if (a.type_p != b.type_p) return a.type_p > b.type_p;
        if (a.id_p != b.id_p) return a.id_p > b.id_p;
        if (a.view_p != b.view_p) return a.view_p > b.view_p;
        return a.scale > b.scale;
    });

    std::vector<int32_t> ids;
    ids.reserve(entries.size());
    for (auto& e : entries) ids.push_back(e.id);
    return alloc_id_handles(as_file(file), ids, out_count);
}

}  // extern "C"
