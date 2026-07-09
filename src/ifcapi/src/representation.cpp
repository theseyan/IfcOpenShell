// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/representation.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/exception.h"

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

bool is_a(express::Base e, const char* name) {
    return e && e.declaration().is(name);
}

int32_t id_of(express::Base e) {
    return e ? static_cast<int32_t>(e.id()) : 0;
}

express::Base read_ref(express::Base e, const char* attr) {
    auto* d = e ? e.declaration().as_entity() : nullptr;
    if (!d) return {};
    int idx = d->attribute_index(attr);
    if (idx < 0) return {};
    try {
        auto val = e.get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return {};
        return static_cast<express::Base>(val);
    } catch (...) {
        return {};
    }
}

std::string read_string(express::Base e, const char* attr) {
    auto* d = e ? e.declaration().as_entity() : nullptr;
    if (!d) return {};
    int idx = d->attribute_index(attr);
    if (idx < 0) return {};
    try {
        auto val = e.get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return {};
        return (std::string)val;
    } catch (...) {
        return {};
    }
}

std::vector<express::Base> read_ref_list(express::Base e, const char* attr) {
    std::vector<express::Base> out;
    auto* d = e ? e.declaration().as_entity() : nullptr;
    if (!d) return out;
    int idx = d->attribute_index(attr);
    if (idx < 0) return out;
    try {
        auto val = e.get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return out;
        return static_cast<std::vector<express::Base>>(val);
    } catch (...) {}
    return out;
}

// Read a numeric attribute; returns default_val when null or missing.
double read_double(express::Base e, const char* attr, double default_val = 0.0) {
    auto* d = e ? e.declaration().as_entity() : nullptr;
    if (!d) return default_val;
    int idx = d->attribute_index(attr);
    if (idx < 0) return default_val;
    try {
        auto val = e.get_attribute_value(static_cast<size_t>(idx));
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

void collect_base_items(express::Base rep, std::vector<express::Base>& out, int depth = 0) {
    if (!rep || depth > 64) return;
    auto items = read_ref_list(rep, "Items");
    std::vector<express::Base> queue(items.begin(), items.end());
    int guard = 0;
    while (!queue.empty() && guard++ < 100000) {
        auto item = queue.back();
        queue.pop_back();
        if (!item) continue;
        if (is_a(item, "IfcMappedItem")) {
            auto src = read_ref(item, "MappingSource");
            if (!src) continue;
            auto mapped = read_ref(src, "MappedRepresentation");
            if (!mapped) continue;
            collect_base_items(mapped, out, depth + 1);
        } else if (is_a(item, "IfcBooleanResult")) {
            auto first = read_ref(item, "FirstOperand");
            auto second = read_ref(item, "SecondOperand");
            if (first) queue.push_back(first);
            if (second) queue.push_back(second);
        } else {
            out.push_back(item);
        }
    }
}

}  // namespace

namespace ifcapi {
namespace bindings {

express::Base representation_get_context(
    ifcopenshell::file* file,
    const char* context_type,
    const char* subcontext,
    const char* target_view)
{
    auto* f = file;
    if (!f) return {};

    bool use_sub = (subcontext && *subcontext) || (target_view && *target_view);
    auto* decl = f->schema()->declaration_by_name(
        use_sub ? "IfcGeometricRepresentationSubContext" : "IfcGeometricRepresentationContext");
    auto insts = f->instances_by_type(decl);

    for (auto e : insts) {
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
        return e;
    }
    return {};
}

express::Base representation_resolve(express::Base* rep) {
    auto e = rep ? *rep : express::Base();
    if (!e) return {};

    // Tekla 2023 workaround: a representation with a single IfcMappedItem whose
    // mapping source points to another representation is unwrapped.
    for (int guard = 0; guard < 64; ++guard) {
        auto items = read_ref_list(e, "Items");
        if (items.size() != 1 || !is_a(items[0], "IfcMappedItem")) break;
        auto src = read_ref(items[0], "MappingSource");
        if (!src) break;
        auto mapped = read_ref(src, "MappedRepresentation");
        if (!mapped) break;
        e = mapped;
    }
    return e;
}

express::Base representation_get_product_representation(
    express::Base* element,
    const RepresentationGetProductRepresentationOptions& options)
{
    auto e = element ? *element : express::Base();
    if (!e) return {};

    std::vector<express::Base> reps;
    if (is_a(e, "IfcProduct")) {
        auto rep = read_ref(e, "Representation");
        if (rep) {
            reps = read_ref_list(rep, "Representations");
        }
    } else if (is_a(e, "IfcTypeProduct")) {
        auto maps = read_ref_list(e, "RepresentationMaps");
        for (auto m : maps) {
            auto r = read_ref(m, "MappedRepresentation");
            if (r) reps.push_back(r);
        }
    }

    auto context_e = options.context.value_or(express::Base());
    auto context_type_str = options.context_type.value_or(std::string());
    auto subcontext_str = options.subcontext.value_or(std::string());
    auto target_view_str = options.target_view.value_or(std::string());

    for (auto r : reps) {
        auto ctx = read_ref(r, "ContextOfItems");
        if (!ctx) continue;

        if (context_e) {
            if (ctx == context_e) return r;
            continue;
        }
        if (!target_view_str.empty()) {
            if (!is_a(ctx, "IfcGeometricRepresentationSubContext")) continue;
            if (read_string(ctx, "TargetView") != target_view_str) continue;
            if (read_string(ctx, "ContextIdentifier") != subcontext_str) continue;
            if (read_string(ctx, "ContextType") != context_type_str) continue;
            return r;
        }
        if (!subcontext_str.empty()) {
            if (!is_a(ctx, "IfcGeometricRepresentationSubContext")) continue;
            if (read_string(ctx, "ContextIdentifier") != subcontext_str) continue;
            if (!str_eq_opt(read_string(ctx, "ContextType"), context_type_str.empty() ? nullptr : context_type_str.c_str())) continue;
            return r;
        }
        if (!str_eq_opt(read_string(ctx, "ContextType"), context_type_str.empty() ? nullptr : context_type_str.c_str())) continue;
        return r;
    }
    return {};
}

std::vector<express::Base> representation_resolve_base_items(express::Base* representation)
{
    auto e = representation ? *representation : express::Base();
    if (!e) return {};
    std::vector<express::Base> result;
    collect_base_items(e, result);
    return result;
}

std::vector<express::Base> representation_get_prioritised_contexts(ifcopenshell::file* file)
{
    auto* f = file;
    if (!f) return {};

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
        express::Base entity;
    };
    std::vector<Entry> entries;

    auto* decl = f->schema()->declaration_by_name("IfcGeometricRepresentationContext");
    auto insts = f->instances_by_type(decl);
    {
        for (auto c : insts) {
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
            e.entity = c;
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

    std::vector<express::Base> result;
    result.reserve(entries.size());
    for (auto& e : entries) result.push_back(e.entity);
    return result;
}

} // namespace bindings
} // namespace ifcapi
