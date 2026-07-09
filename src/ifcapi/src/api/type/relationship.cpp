// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"

#include <algorithm>
#include <cstring>
#include <set>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

// Find the IfcRelDefinesByType that the given type is the RelatingType of.
// For IFC2X3, the inverse is "ObjectTypeOf"; for IFC4+, it is "Types".
static express::Base find_types_rel(express::Base relating_type) {
    // Try IFC4+ "Types" first, then IFC2X3 "ObjectTypeOf"
    for (const char* inverse_name : {"Types", "ObjectTypeOf"}) {
        auto result = ifcapi::detail::read_inverse_aggregate(relating_type, inverse_name);
        if (!result.empty()) return result.front();
    }
    return {};
}

// Find the IfcRelDefinesByType that the element is typed by.
// For IFC4+: inverse "IsTypedBy"; for IFC2X3: filter "IsDefinedBy" for IfcRelDefinesByType.
static express::Base find_element_type_rel(ifcopenshell::file* file, express::Base element) {
    auto result = ifcapi::detail::read_inverse_aggregate(element, "IsTypedBy");
    if (!result.empty()) return result.front();

    // IFC2X3: IsDefinedBy, filter for IfcRelDefinesByType
    result = ifcapi::detail::read_inverse_aggregate(element, "IsDefinedBy");
    const auto* rdt_decl = file->schema()->declaration_by_name("IfcRelDefinesByType");
    for (auto inverse : result) {
        if (inverse && inverse.declaration().is(*rdt_decl)) {
            return inverse;
        }
    }

    return {};
}

static std::set<express::Base> collect_previous_type_rels(
    ifcopenshell::file* file,
    const std::set<express::Base>& objects_set,
    express::Base existing_rel)
{
    std::set<express::Base> previous_rels;
    for (auto obj : objects_set) {
        auto rel = find_element_type_rel(file, obj);
        if (rel && !ifcapi::detail::same_instance(rel, existing_rel)) {
            previous_rels.insert(rel);
        }
    }
    return previous_rels;
}

namespace {
using namespace ifcapi::detail;

// Strip ObjectType / PredefinedType from related objects when the relating
// type already declares a non-NOTDEFINED PredefinedType (avoids "double
// typing"; see ifcopenshell issue 7006).
void clear_predefined_type_on_objects(
    const std::set<express::Base>& objects_set,
    express::Base relating_type)
{
    auto* d = relating_type ? relating_type.declaration().as_entity() : nullptr;
    if (!d) return;
    int pdt_idx = d->attribute_index("PredefinedType");
    if (pdt_idx < 0) return;
    std::string predefined;
    try {
        auto v = relating_type.get_attribute_value(static_cast<size_t>(pdt_idx));
        if (v.isNull()) return;
        predefined = (std::string)v;
    } catch (...) { return; }
    if (predefined.empty() || predefined == "NOTDEFINED") return;

    for (auto obj : objects_set) {
        auto* od = obj ? obj.declaration().as_entity() : nullptr;
        if (!od) continue;
        int ot_idx = od->attribute_index("ObjectType");
        if (ot_idx >= 0) {
            try { obj.set_attribute_value(static_cast<size_t>(ot_idx), blank{}); } catch (...) {}
        }
        int p_idx = od->attribute_index("PredefinedType");
        if (p_idx >= 0) {
            try { obj.set_attribute_value(static_cast<size_t>(p_idx), blank{}); } catch (...) {}
        }
    }
}

express::Base assign_type_core(
    ifcopenshell::file* file,
    const std::vector<express::Base>& objects,
    express::Base relating_type,
    bool should_map_representations,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    if (!file || objects.empty()) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto relating_type_e = relating_type;
        if (!relating_type_e) {
            set_error("Relating type not found");
            return {};
        }

        std::set<express::Base> objects_set;
        for (auto object : objects) {
            if (object) objects_set.insert(object);
        }
        if (objects_set.empty()) return {};

        auto existing_rel = find_types_rel(relating_type_e);

        const auto* rdt_decl = file->schema()->declaration_by_name("IfcRelDefinesByType");
        auto* rdt_entity_decl = rdt_decl->as_entity();
        int related_idx = find_attr_index(rdt_entity_decl, "RelatedObjects");

        std::vector<express::Base> objects_to_change;

        for (auto obj : objects_set) {
            auto cur_rel = find_element_type_rel(file, obj);
            if (!cur_rel) {
                objects_to_change.push_back(obj);
            } else if (!ifcapi::detail::same_instance(cur_rel, existing_rel)) {
                objects_to_change.push_back(obj);
            }
        }

        if (objects_to_change.empty()) {
            return existing_rel;
        }

        // Remove from previous type relationships.
        auto previous_rels = collect_previous_type_rels(file, objects_set, existing_rel);
        for (auto prev_rel : previous_rels) {
            auto related = get_ref_aggregate(prev_rel, related_idx);
            std::vector<express::Base> remaining;
            for (auto e : related) {
                if (objects_set.find(e) == objects_set.end()) {
                    remaining.push_back(e);
                }
            }
            if (remaining.empty()) {
                remove_with_history(file, prev_rel);
            } else {
                set_ref_aggregate(prev_rel, related_idx, remaining);
                update_owner_history(file, prev_rel, user, application);
            }
        }

        express::Base result_rel;

        // Add to target type relationship.
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<express::Base> current_set(current.begin(), current.end());
            for (auto o : objects_set) current_set.insert(o);
            std::vector<express::Base> merged(current_set.begin(), current_set.end());
            set_ref_aggregate(existing_rel, related_idx, merged);
            update_owner_history(file, existing_rel, user, application);
            result_rel = existing_rel;
        } else {
            auto rel = file->create(rdt_decl);
            if (!rel) {
                set_error("Failed to create IfcRelDefinesByType");
                return {};
            }
            int gi_idx = find_attr_index(rdt_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel.set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int rt_idx = find_attr_index(rdt_entity_decl, "RelatingType");
            set_ref(rel, rt_idx, relating_type_e);
            int oh_idx = find_attr_index(rdt_entity_decl, "OwnerHistory");
            set_ref(rel, oh_idx, ensure_owner_history(file, owner_history, user, application));
            std::vector<express::Base> objs(objects_set.begin(), objects_set.end());
            set_ref_aggregate(rel, related_idx, objs);
            result_rel = rel;
        }

        if (should_map_representations) {
            // Propagate IfcRepresentationMaps onto each newly-assigned object.
            if (relating_type_e) {
                int rm_idx = relating_type_e.declaration().as_entity()->attribute_index("RepresentationMaps");
                bool has_maps = false;
                if (rm_idx >= 0) {
                    try {
                        auto v = relating_type_e.get_attribute_value(static_cast<size_t>(rm_idx));
                        if (!v.isNull()) {
                            has_maps = !static_cast<std::vector<express::Base>>(v).empty();
                        }
                    } catch (...) {}
                }
                if (has_maps) {
                    for (auto obj : objects_to_change) {
                        ifcapi::bindings::type_map_type_representations(file, &obj, &relating_type_e);
                    }
                }
            }

            clear_predefined_type_on_objects(objects_set, relating_type_e);
        }

        return result_rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}
}  // namespace

namespace ifcapi {
namespace bindings {

express::Base type_assign_type(
    ifcopenshell::file* file,
    const TypeAssignTypeOptions& options)
{
    return assign_type_core(
        file,
        options.objects,
        options.relating_type,
        options.should_map_representations.value_or(true),
        options.owner_history.value_or(express::Base()),
        options.user.value_or(express::Base()),
        options.application.value_or(express::Base()));
}

void type_unassign_type(
    ifcopenshell::file* file,
    const TypeUnassignTypeOptions& options)
{
    if (!file || options.objects.empty()) return;

    try {
        const auto* rdt_decl = file->schema()->declaration_by_name("IfcRelDefinesByType");
        auto* rdt_entity_decl = rdt_decl->as_entity();
        int related_idx = find_attr_index(rdt_entity_decl, "RelatedObjects");

        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        std::set<express::Base> objects_set;
        for (auto object : options.objects) {
            if (object) objects_set.insert(object);
        }

        std::set<express::Base> rels;
        for (auto obj : objects_set) {
            auto rel = find_element_type_rel(file, obj);
            if (rel) rels.insert(rel);
        }

        for (auto rel : rels) {
            auto related = get_ref_aggregate(rel, related_idx);
            std::vector<express::Base> remaining;
            for (auto e : related) {
                if (objects_set.find(e) == objects_set.end()) {
                    remaining.push_back(e);
                }
            }
            if (remaining.empty()) {
                remove_with_history(file, rel);
            } else {
                set_ref_aggregate(rel, related_idx, remaining);
                update_owner_history(file, rel, user_value, application_value);
            }
        }
    } catch (...) {}
}

} // namespace bindings
} // namespace ifcapi
