// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/pset.h"
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
static IfcUtil::IfcBaseClass* find_is_grouped_by(IfcUtil::IfcBaseClass* group) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(group);
    if (!be) return nullptr;
    try {
        auto result = be->get_inverse("IsGroupedBy");
        if (result && result->size() > 0) return (*result)[0];
    } catch (...) {}
    return nullptr;
}

static std::vector<IfcUtil::IfcBaseClass*> get_group_rels(IfcUtil::IfcBaseClass* group) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(group);
    if (!be) return result;
    try {
        auto rels = be->get_inverse("IsGroupedBy");
        if (rels) {
            for (size_t i = 0; i < rels->size(); ++i) {
                if ((*rels)[i]) result.push_back((*rels)[i]);
            }
        }
    } catch (...) {}
    return result;
}

static IfcUtil::IfcBaseClass* create_group_rel(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* group,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const auto* rel_decl = file->schema()->declaration_by_name("IfcRelAssignsToGroup");
    auto* rel = file->create(rel_decl);
    auto* rel_entity_decl = rel_decl->as_entity();
    int gi_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "GlobalId");
    if (gi_idx >= 0) rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
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

IfcUtil::IfcBaseClass* group_add_group(
    IfcParse::IfcFile* file,
    const std::string& name,
    const char* description,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        const auto* group_decl = file->schema()->declaration_by_name("IfcGroup");
        auto* group = file->create(group_decl);
        auto* group_entity_decl = group_decl->as_entity();
        int gi_idx = find_attr_index(group_entity_decl, "GlobalId");
        if (gi_idx >= 0) group->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
        int oh_idx = find_attr_index(group_entity_decl, "OwnerHistory");
        set_ref(group, oh_idx, ensure_owner_history(file, owner_history, user, application));
        int name_idx = find_attr_index(group_entity_decl, "Name");
        if (name_idx >= 0) group->set_attribute_value(static_cast<size_t>(name_idx), name);
        if (description) {
            int desc_idx = find_attr_index(group_entity_decl, "Description");
            if (desc_idx >= 0) group->set_attribute_value(static_cast<size_t>(desc_idx), std::string(description));
        }
        return group;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* group_update_group_products(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* group,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file || !group) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        std::vector<IfcUtil::IfcBaseClass*> product_vec;
        for (auto* product : products) {
            if (product) product_vec.push_back(const_cast<IfcUtil::IfcBaseClass*>(product));
        }

        auto rels = get_group_rels(group);
        if (rels.empty()) {
            return create_group_rel(file, group, product_vec, owner_history, user, application);
        }

        std::vector<IfcUtil::IfcBaseClass*> related;
        std::set<IfcUtil::IfcBaseClass*> seen;
        for (auto* product : product_vec) {
            if (seen.insert(product).second) related.push_back(product);
        }

        for (auto* rel : rels) {
            int related_idx = find_attr_index(rel->declaration().as_entity(), "RelatedObjects");
            for (auto* item : get_ref_aggregate(rel, related_idx)) {
                if (item && item->declaration().is("IfcGroup") && seen.insert(item).second) {
                    related.push_back(item);
                }
            }
        }

        for (size_t i = 1; i < rels.size(); ++i) {
            remove_with_history(file, rels[i]);
        }

        auto* rel = rels.front();
        int related_idx = find_attr_index(rel->declaration().as_entity(), "RelatedObjects");
        set_ref_aggregate(rel, related_idx, related);
        update_owner_history(file, rel, user, application);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* group_assign_group(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* group,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file || products.empty()) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        auto* group_e = group;
        if (!group_e) {
            set_error("Group not found");
            return nullptr;
        }

        std::vector<IfcUtil::IfcBaseClass*> products_vec;
        for (auto* obj : products) {
            if (obj) products_vec.push_back(const_cast<IfcUtil::IfcBaseClass*>(obj));
        }
        if (products_vec.empty()) return nullptr;

        auto* existing_rel = find_is_grouped_by(group_e);

        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelAssignsToGroup");
        auto* rel_entity_decl = rel_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        if (!existing_rel) {
            return create_group_rel(file, group_e, products_vec, owner_history, user, application);
        }

        // Merge into existing relationship
        auto current = get_ref_aggregate(existing_rel, related_idx);
        std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());

        std::set<IfcUtil::IfcBaseClass*> products_set(products_vec.begin(), products_vec.end());
        bool all_present = true;
        for (auto* p : products_set) {
            if (current_set.find(p) == current_set.end()) {
                all_present = false;
                break;
            }
        }
        if (all_present) {
            return existing_rel;
        }

        for (auto* p : products_vec) {
            if (current_set.insert(p).second) {
                current.push_back(p);
            }
        }
        set_ref_aggregate(existing_rel, related_idx, current);
        update_owner_history(file, existing_rel, user, application);
        return existing_rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void group_unassign_group(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* group,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || products.empty() || !group) return;

    try {
        auto* group_e = group;
        if (!group_e) return;

        auto* rel = find_is_grouped_by(group_e);
        if (!rel) return;

        auto* rel_entity_decl = rel->declaration().as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (auto* obj : products) {
            if (obj) products_set.insert(const_cast<IfcUtil::IfcBaseClass*>(obj));
        }

        auto related = get_ref_aggregate(rel, related_idx);
        std::vector<IfcUtil::IfcBaseClass*> remaining;
        for (auto* e : related) {
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
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* group)
{
    if (!file || !group) return;

    std::vector<int> inverse_ids;
    try {
        auto inverses = file->getInverse(group->id(), nullptr, -1);
        if (inverses) {
            for (auto* inverse : *inverses) {
                if (inverse && inverse->id() > 0) inverse_ids.push_back(inverse->id());
            }
        }
    } catch (...) {
    }

    for (int inverse_id : inverse_ids) {
        auto* inverse = file->instance_by_id(inverse_id);
        if (!inverse) continue;
        if (inverse->declaration().is("IfcRelDefinesByProperties")) {
            auto* pset = read_ref_attr(inverse, "RelatingPropertyDefinition");
            if (pset) pset_remove_pset(file, group, pset);
        } else if (inverse->declaration().is("IfcRelAssignsToGroup")) {
            auto* relating_group = read_ref_attr(inverse, "RelatingGroup");
            auto related = read_ref_aggregate(inverse, "RelatedObjects");
            if (relating_group == group || related.size() == 1) {
                remove_with_history(file, inverse);
            }
        }
    }

    remove_with_history(file, group);
}

} // namespace bindings
} // namespace ifcapi
