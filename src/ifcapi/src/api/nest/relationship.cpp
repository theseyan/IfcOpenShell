// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"

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

// Find IfcRelNests where this entity is the RelatingObject (the whole).
// IFC4+: inverse "IsNestedBy"; IFC2X3: filter "IsDecomposedBy" for IfcRelNests.
static express::Base find_is_nested_by(ifcopenshell::file* file, express::Base entity) {
    auto result = ifcapi::detail::read_inverse_aggregate(entity, "IsNestedBy");
    if (!result.empty()) return result.front();
    // IFC2X3: filter IsDecomposedBy for IfcRelNests
    result = ifcapi::detail::read_inverse_aggregate(entity, "IsDecomposedBy");
    const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
    for (auto inverse : result) {
        if (inverse && inverse.declaration().is(*nests_decl)) {
            return inverse;
        }
    }
    return {};
}

// Find IfcRelNests where this entity is a nested child.
// IFC4+: inverse "Nests"; IFC2X3: filter "Decomposes" for IfcRelNests.
static express::Base find_nests(ifcopenshell::file* file, express::Base entity) {
    auto result = ifcapi::detail::read_inverse_aggregate(entity, "Nests");
    if (!result.empty()) return result.front();
    // IFC2X3: filter Decomposes for IfcRelNests
    result = ifcapi::detail::read_inverse_aggregate(entity, "Decomposes");
    const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
    for (auto inverse : result) {
        if (inverse && inverse.declaration().is(*nests_decl)) {
            return inverse;
        }
    }
    return {};
}

static std::set<express::Base> collect_previous_nest_rels(
    ifcopenshell::file* file,
    const std::set<express::Base>& objects_set,
    express::Base existing_rel)
{
    std::set<express::Base> previous_rels;
    for (auto obj : objects_set) {
        auto rel = find_nests(file, obj);
        if (rel && !ifcapi::detail::same_instance(rel, existing_rel)) {
            previous_rels.insert(rel);
        }
    }
    return previous_rels;
}

namespace ifcapi {
namespace bindings {
using namespace ifcapi::detail;

express::Base nest_assign_object(
    ifcopenshell::file* file,
    const std::vector<express::Base>& objects,
    express::Base* relating_object,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    ifcopenshell_clear_error();
    if (!file || objects.empty()) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto relating = deref_or_empty(relating_object);
        if (!relating) {
            set_error("Relating object not found");
            return {};
        }

        // Maintain insertion order (nesting order matters in IFC).
        std::vector<express::Base> objects_vec;
        std::set<express::Base> objects_set;
        for (auto object : objects) {
            if (object && objects_set.insert(object).second) {
                objects_vec.push_back(object);
            }
        }
        if (objects_vec.empty()) return {};

        auto existing_rel = find_is_nested_by(file, relating);
        auto owner_history_value = deref_or_empty(owner_history);
        auto user_value = deref_or_empty(user);
        auto application_value = deref_or_empty(application);

        const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
        auto* nests_entity_decl = nests_decl->as_entity();
        int related_idx = find_attr_index(nests_entity_decl, "RelatedObjects");

        std::vector<express::Base> objects_to_change;

        for (auto obj : objects_vec) {
            auto cur_rel = find_nests(file, obj);
            if (!cur_rel) {
                objects_to_change.push_back(obj);
            } else if (!ifcapi::detail::same_instance(cur_rel, existing_rel)) {
                objects_to_change.push_back(obj);
            }
        }

        if (objects_to_change.empty()) {
            return existing_rel;
        }

        // Unassign from spatial containers.
        {
            std::vector<express::Base> contained_objects;
            for (auto obj : objects_to_change) {
                if (!ifcapi::detail::read_inverse_aggregate(obj, "ContainedInStructure").empty()) {
                    contained_objects.push_back(obj);
                }
            }
            if (!contained_objects.empty()) {
                spatial_unassign_container(file, contained_objects, user, application);
            }
        }

        // Unassign from aggregates.
        {
            aggregate_unassign_object(file, objects_to_change, user, application);
        }

        // Remove from previous nest rels (preserving order).
        auto previous_rels = collect_previous_nest_rels(file, objects_set, existing_rel);
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
                update_owner_history(file, prev_rel, user_value, application_value);
            }
        }

        // Add to target nesting (preserving order).
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<express::Base> current_set(current.begin(), current.end());
            for (auto o : objects_vec) {
                if (current_set.insert(o).second) {
                    current.push_back(o);
                }
            }
            set_ref_aggregate(existing_rel, related_idx, current);
            update_owner_history(file, existing_rel, user_value, application_value);
            return existing_rel;
        } else {
            auto rel = file->create(nests_decl);
            if (!rel) {
                set_error("Failed to create IfcRelNests");
                return {};
            }
            int gi_idx = find_attr_index(nests_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel.set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int ro_idx = find_attr_index(nests_entity_decl, "RelatingObject");
            set_ref(rel, ro_idx, relating);
            int oh_idx = find_attr_index(nests_entity_decl, "OwnerHistory");
            set_ref(rel, oh_idx, ensure_owner_history(file, owner_history_value, user_value, application_value));
            set_ref_aggregate(rel, related_idx, objects_vec);
            return rel;
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void nest_unassign_object(
    ifcopenshell::file* file,
    const std::vector<express::Base>& objects,
    express::Base* user,
    express::Base* application)
{
    if (!file || objects.empty()) return;

    try {
        auto user_value = deref_or_empty(user);
        auto application_value = deref_or_empty(application);
        const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
        auto* nests_entity_decl = nests_decl->as_entity();
        int related_idx = find_attr_index(nests_entity_decl, "RelatedObjects");

        std::set<express::Base> objects_set;
        for (auto object : objects) {
            if (object) objects_set.insert(object);
        }

        std::set<express::Base> rels;
        for (auto obj : objects_set) {
            auto rel = find_nests(file, obj);
            if (rel) rels.insert(rel);
        }

        for (auto rel : rels) {
            auto related = get_ref_aggregate(rel, related_idx);
            // Preserve order while removing
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
