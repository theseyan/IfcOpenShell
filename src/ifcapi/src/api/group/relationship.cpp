// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"

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

// Find the IfcRelAssignsToGroup where this group is the RelatingGroup.
static express::Base find_is_grouped_by(express::Base group) {
    auto result = ifcapi::detail::read_inverse_aggregate(group, "IsGroupedBy");
    return result.empty() ? express::Base() : result.front();
}

static std::vector<express::Base> get_group_rels(express::Base group) {
    return ifcapi::detail::read_inverse_aggregate(group, "IsGroupedBy");
}

static express::Base create_group_rel(
    ifcopenshell::file* file,
    express::Base group,
    const std::vector<express::Base>& products,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* rel_decl = file->schema()->declaration_by_name("IfcRelAssignsToGroup");
    auto rel = file->create(rel_decl);
    auto* rel_entity_decl = rel_decl->as_entity();
    int gi_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "GlobalId");
    if (gi_idx >= 0) rel.set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
    int rg_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "RelatingGroup");
    ifcapi::detail::set_ref(rel, rg_idx, group);
    int oh_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "OwnerHistory");
    ifcapi::detail::set_ref(rel, oh_idx, ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    int related_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "RelatedObjects");
    ifcapi::detail::set_ref_aggregate(rel, related_idx, products);
    return rel;
}

namespace ifcapi {
namespace bindings {
using namespace ifcapi::detail;

express::Base group_add_group(
    ifcopenshell::file* file,
    const GroupAddGroupOptions& options)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto owner_history = options.owner_history.value_or(express::Base());
        auto user = options.user.value_or(express::Base());
        auto application = options.application.value_or(express::Base());

        const auto* group_decl = file->schema()->declaration_by_name("IfcGroup");
        auto group = file->create(group_decl);
        auto* group_entity_decl = group_decl->as_entity();
        int gi_idx = find_attr_index(group_entity_decl, "GlobalId");
        if (gi_idx >= 0) group.set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
        int oh_idx = find_attr_index(group_entity_decl, "OwnerHistory");
        set_ref(group, oh_idx, ensure_owner_history(file, owner_history, user, application));
        int name_idx = find_attr_index(group_entity_decl, "Name");
        if (name_idx >= 0) group.set_attribute_value(static_cast<size_t>(name_idx), options.name);
        if (options.description.has_value()) {
            int desc_idx = find_attr_index(group_entity_decl, "Description");
            if (desc_idx >= 0) group.set_attribute_value(static_cast<size_t>(desc_idx), options.description.value());
        }
        return group;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

express::Base group_update_group_products(
    ifcopenshell::file* file,
    const GroupUpdateGroupProductsOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !options.group) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto owner_history = options.owner_history.value_or(express::Base());
        auto user = options.user.value_or(express::Base());
        auto application = options.application.value_or(express::Base());

        std::vector<express::Base> product_vec;
        for (auto product : options.products) if (product) product_vec.push_back(product);

        auto rels = get_group_rels(options.group);
        if (rels.empty()) {
            return create_group_rel(file, options.group, product_vec, owner_history, user, application);
        }

        std::vector<express::Base> related;
        std::set<express::Base> seen;
        for (auto product : product_vec) {
            if (seen.insert(product).second) related.push_back(product);
        }

        for (auto rel : rels) {
            int related_idx = find_attr_index(rel.declaration().as_entity(), "RelatedObjects");
            for (auto item : get_ref_aggregate(rel, related_idx)) {
                if (item && item.declaration().is("IfcGroup") && seen.insert(item).second) {
                    related.push_back(item);
                }
            }
        }

        for (size_t i = 1; i < rels.size(); ++i) {
            remove_with_history(file, rels[i]);
        }

        auto rel = rels.front();
        int related_idx = find_attr_index(rel.declaration().as_entity(), "RelatedObjects");
        set_ref_aggregate(rel, related_idx, related);
        update_owner_history(file, rel, user, application);
        return rel;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

express::Base group_assign_group(
    ifcopenshell::file* file,
    const GroupAssignGroupOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || options.products.empty()) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto group_e = options.group;
        if (!group_e) {
            set_error("Group not found");
            return {};
        }

        auto owner_history = options.owner_history.value_or(express::Base());
        auto user = options.user.value_or(express::Base());
        auto application = options.application.value_or(express::Base());

        std::vector<express::Base> products_vec;
        for (auto obj : options.products) {
            if (obj) products_vec.push_back(obj);
        }
        if (products_vec.empty()) return {};

        auto existing_rel = find_is_grouped_by(group_e);

        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelAssignsToGroup");
        auto* rel_entity_decl = rel_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        if (!existing_rel) {
            return create_group_rel(file, group_e, products_vec, owner_history, user, application);
        }

        // Merge into existing relationship
        auto current = get_ref_aggregate(existing_rel, related_idx);
        std::set<express::Base> current_set(current.begin(), current.end());

        std::set<express::Base> products_set(products_vec.begin(), products_vec.end());
        bool all_present = true;
        for (auto p : products_set) {
            if (current_set.find(p) == current_set.end()) {
                all_present = false;
                break;
            }
        }
        if (all_present) {
            return existing_rel;
        }

        for (auto p : products_vec) {
            if (current_set.insert(p).second) {
                current.push_back(p);
            }
        }
        set_ref_aggregate(existing_rel, related_idx, current);
        update_owner_history(file, existing_rel, user, application);
        return existing_rel;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

void group_unassign_group(
    ifcopenshell::file* file,
    const GroupUnassignGroupOptions& options)
{
    if (!file || options.products.empty() || !options.group) return;

    try {
        auto user = options.user.value_or(express::Base());
        auto application = options.application.value_or(express::Base());

        auto group_e = options.group;
        if (!group_e) return;

        auto rel = find_is_grouped_by(group_e);
        if (!rel) return;

        auto* rel_entity_decl = rel.declaration().as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        std::set<express::Base> products_set;
        for (auto obj : options.products) {
            if (obj) products_set.insert(obj);
        }

        auto related = get_ref_aggregate(rel, related_idx);
        std::vector<express::Base> remaining;
        for (auto e : related) {
            if (products_set.find(e) == products_set.end()) {
                remaining.push_back(e);
            }
        }

        if (remaining.empty()) {
            remove_with_history(file, rel);
        } else {
            set_ref_aggregate(rel, related_idx, remaining);
            update_owner_history(file, rel, user, application);
        }
    } catch (...) {}
}

void group_remove_group(
    ifcopenshell::file* file,
    express::Base* group)
{
    if (!file || !group) return;

    std::vector<int> inverse_ids;
    try {
        for (auto inverse : file->instances_by_reference(static_cast<int>(group->id()))) {
            if (inverse && inverse.id() > 0) inverse_ids.push_back(static_cast<int>(inverse.id()));
        }
    } catch (...) {
    }

    for (int inverse_id : inverse_ids) {
        auto inverse = file->instance_by_id(inverse_id);
        if (!inverse) continue;
        if (inverse.declaration().is("IfcRelDefinesByProperties")) {
            auto pset = read_ref_attr(inverse, "RelatingPropertyDefinition");
            if (pset) pset_remove_pset(file, group, &pset);
        } else if (inverse.declaration().is("IfcRelAssignsToGroup")) {
            auto relating_group = read_ref_attr(inverse, "RelatingGroup");
            auto related = read_ref_aggregate(inverse, "RelatedObjects");
            if (relating_group == *group || related.size() == 1) {
                remove_with_history(file, inverse);
            }
        }
    }

    remove_with_history(file, *group);
}

void group_edit_group(
    ifcopenshell::file* file,
    express::Base group,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, group, attributes);
}

} // namespace bindings
} // namespace ifcapi
