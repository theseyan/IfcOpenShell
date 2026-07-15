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
#include <stdexcept>
#include <set>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

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

static void require_owned(ifcopenshell::file* file, express::Base entity, const char* name) {
    if (!entity || entity.file() != file) {
        throw std::runtime_error(std::string(name) + " must belong to the target file");
    }
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
    const NestAssignObjectOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || options.products.empty()) {
        throw std::invalid_argument("nest_assign_object requires a file and at least one product");
    }

    try {
        auto relating = options.relating_object;
        require_owned(file, relating, "Relating object");

        // Maintain insertion order (nesting order matters in IFC).
        std::vector<express::Base> objects_vec;
        std::set<express::Base> objects_set;
        for (auto object : options.products) {
            require_owned(file, object, "Nested object");
            if (objects_set.insert(object).second) {
                objects_vec.push_back(object);
            }
        }
        if (objects_vec.empty()) return {};

        auto existing_rel = find_is_nested_by(file, relating);
        auto owner_history_value = options.owner_history.value_or(express::Base());
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());

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
                SpatialUnassignContainerOptions spatial_opts;
                spatial_opts.products = contained_objects;
                if (user_value) spatial_opts.user = user_value;
                if (application_value) spatial_opts.application = application_value;
                spatial_unassign_container(file, spatial_opts);
            }
        }

        // Unassign from aggregates.
        {
            AggregateUnassignObjectOptions agg_opts;
            agg_opts.products = objects_to_change;
            if (user_value) agg_opts.user = user_value;
            if (application_value) agg_opts.application = application_value;
            aggregate_unassign_object(file, agg_opts);
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
                throw std::runtime_error("Failed to create IfcRelNests");
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
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

void nest_unassign_object(
    ifcopenshell::file* file,
    const NestUnassignObjectOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || options.products.empty()) return;

    try {
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
        auto* nests_entity_decl = nests_decl->as_entity();
        int related_idx = find_attr_index(nests_entity_decl, "RelatedObjects");

        std::set<express::Base> objects_set;
        for (auto object : options.products) {
            require_owned(file, object, "Nested object");
            objects_set.insert(object);
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
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

void nest_change_nest(ifcopenshell::file* file, const NestChangeNestOptions& options) {
    ifcopenshell_clear_error();
    try {
        if (!file) throw std::runtime_error("nest_change_nest requires a file");
        require_owned(file, options.item, "Item");
        require_owned(file, options.new_parent, "New parent");
        auto old_rel = find_nests(file, options.item);
        if (!old_rel) return;

        auto related = read_ref_aggregate(old_rel, "RelatedObjects");
        std::vector<express::Base> remaining;
        for (auto child : related) {
            if (!same_instance(child, options.item)) remaining.push_back(child);
        }
        auto user = options.user.value_or(express::Base());
        auto application = options.application.value_or(express::Base());
        if (remaining.empty()) {
            remove_with_history(file, old_rel);
        } else {
            write_ref_aggregate(old_rel, "RelatedObjects", remaining);
            update_owner_history(file, old_rel, user, application);
        }

        NestAssignObjectOptions assign;
        assign.products = {options.item};
        assign.relating_object = options.new_parent;
        assign.owner_history = options.owner_history;
        assign.user = options.user;
        assign.application = options.application;
        auto result = nest_assign_object(file, assign);
        if (!result) {
            auto message = ifcopenshell_last_error_message();
            throw std::runtime_error(message && *message ? message : "Failed to assign new nest");
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

void nest_reorder_nesting(ifcopenshell::file* file, const NestReorderNestingOptions& options) {
    ifcopenshell_clear_error();
    try {
        if (!file) throw std::runtime_error("nest_reorder_nesting requires a file");
        require_owned(file, options.item, "Item");
        auto rel = find_nests(file, options.item);
        if (!rel) return;
        auto items = read_ref_aggregate(rel, "RelatedObjects");
        if (items.empty()) return;

        long old_index = 0;
        if (options.old_index) {
            old_index = *options.old_index;
            if (old_index < 0) old_index += static_cast<long>(items.size());
        } else {
            auto it = std::find_if(items.begin(), items.end(), [&](express::Base value) {
                return same_instance(value, options.item);
            });
            if (it == items.end()) throw std::runtime_error("Item is not present in its nesting relationship");
            old_index = std::distance(items.begin(), it);
        }
        if (old_index < 0 || old_index >= static_cast<long>(items.size())) {
            throw std::out_of_range("old_index is outside RelatedObjects");
        }

        auto moved = items[static_cast<size_t>(old_index)];
        items.erase(items.begin() + old_index);
        long new_index = options.new_index.value_or(0);
        if (new_index < 0) new_index = std::max<long>(0, static_cast<long>(items.size()) + new_index);
        if (new_index > static_cast<long>(items.size())) new_index = static_cast<long>(items.size());
        items.insert(items.begin() + new_index, moved);
        write_ref_aggregate(rel, "RelatedObjects", items);
        update_owner_history(file, rel, options.user.value_or(express::Base()), options.application.value_or(express::Base()));
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

} // namespace bindings
} // namespace ifcapi
