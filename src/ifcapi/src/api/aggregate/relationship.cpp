// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <algorithm>
#include <cstring>
#include <set>
#include <string>
#include <vector>

// Shared error helpers (defined in root.cpp)
#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

// Helper: get the IfcRelAggregates inverse for "IsDecomposedBy" on an entity.
static IfcUtil::IfcBaseClass* find_is_decomposed_by(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!be) return nullptr;
    try {
        auto result = be->get_inverse("IsDecomposedBy");
        if (result && result->size() > 0) {
            for (size_t i = 0; i < result->size(); ++i) {
                if ((*result)[i]->declaration().is(*(file->schema()->declaration_by_name("IfcRelAggregates")))) {
                    return (*result)[i];
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

// Helper: get the IfcRelAggregates that this entity "Decomposes" into.
static IfcUtil::IfcBaseClass* find_decomposes(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!be) return nullptr;
    try {
        auto result = be->get_inverse("Decomposes");
        if (result && result->size() > 0) {
            for (size_t i = 0; i < result->size(); ++i) {
                if ((*result)[i]->declaration().is(*(file->schema()->declaration_by_name("IfcRelAggregates")))) {
                    return (*result)[i];
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

// Helper: get the IfcRelContainedInSpatialStructure for "ContainedInStructure" on an entity.
static IfcUtil::IfcBaseClass* find_contained_in_structure(IfcParse::IfcFile* /*file*/, IfcUtil::IfcBaseClass* entity) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!be) return nullptr;
    try {
        auto result = be->get_inverse("ContainedInStructure");
        if (result && result->size() > 0) return (*result)[0];
    } catch (...) {}
    return nullptr;
}

// Helper: get the IfcRelContainedInSpatialStructure for "ContainsElements" on a spatial structure.
static IfcUtil::IfcBaseClass* find_contains_elements(IfcParse::IfcFile* /*file*/, IfcUtil::IfcBaseClass* entity) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!be) return nullptr;
    try {
        auto result = be->get_inverse("ContainsElements");
        if (result && result->size() > 0) return (*result)[0];
    } catch (...) {}
    return nullptr;
}

namespace ifcapi {
namespace bindings {
using namespace ifcapi::detail;

IfcUtil::IfcBaseClass* aggregate_assign_object(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* relating_object,
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
        auto* relating = relating_object;
        if (!relating) {
            set_error("Relating object not found");
            return nullptr;
        }

        // Collect product entities.
        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (auto* product : products) {
            if (product) products_set.insert(const_cast<IfcUtil::IfcBaseClass*>(product));
        }
        if (products_set.empty()) return nullptr;

        // Find existing IfcRelAggregates on the relating object.
        auto* existing_rel = find_is_decomposed_by(file, relating);

        const auto* rel_agg_decl = file->schema()->declaration_by_name("IfcRelAggregates");
        auto* rel_entity_decl = rel_agg_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        // Determine which products need to change.
        std::set<IfcUtil::IfcBaseClass*> previous_rels;
        std::vector<IfcUtil::IfcBaseClass*> products_to_change;

        for (auto* product : products_set) {
            auto* cur_rel = find_decomposes(file, product);
            if (cur_rel == nullptr) {
                products_to_change.push_back(product);
            } else if (cur_rel != existing_rel) {
                previous_rels.insert(cur_rel);
                products_to_change.push_back(product);
            }
            // If cur_rel == existing_rel, product is already correctly assigned — skip.
        }

        if (products_to_change.empty()) {
            return existing_rel;
        }

        // Unassign from spatial containers (products that aren't already aggregated).
        for (auto* product : products_to_change) {
            auto* container_rel = find_contained_in_structure(file, product);
            if (!container_rel) continue;
            auto* container_decl = container_rel->declaration().as_entity();
            int re_idx = find_attr_index(container_decl, "RelatedElements");
            auto elems = get_ref_aggregate(container_rel, re_idx);
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* e : elems) {
                if (products_set.find(e) == products_set.end()) {
                    remaining.push_back(e);
                }
            }
            if (remaining.empty()) {
                remove_with_history(file, container_rel);
            } else {
                set_ref_aggregate(container_rel, re_idx, remaining);
                update_owner_history(file, container_rel, user, application);
            }
        }

        // Remove from previous aggregates.
        for (auto* prev_rel : previous_rels) {
            auto related = get_ref_aggregate(prev_rel, related_idx);
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* e : related) {
                if (products_set.find(e) == products_set.end()) {
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

        // Add to target aggregate.
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());
            for (auto* p : products_set) current_set.insert(p);
            std::vector<IfcUtil::IfcBaseClass*> merged(current_set.begin(), current_set.end());
            set_ref_aggregate(existing_rel, related_idx, merged);
            update_owner_history(file, existing_rel, user, application);
            return existing_rel;
        } else {
            // Create new IfcRelAggregates.
            auto* rel = file->create(rel_agg_decl);
            if (!rel) {
                set_error("Failed to create IfcRelAggregates");
                return nullptr;
            }
            int gi_idx = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int ro_idx = find_attr_index(rel_entity_decl, "RelatingObject");
            set_ref(rel, ro_idx, relating);
            int oh_idx = find_attr_index(rel_entity_decl, "OwnerHistory");
            set_ref(rel, oh_idx, ensure_owner_history(file, owner_history, user, application));
            std::vector<IfcUtil::IfcBaseClass*> prods(products_set.begin(), products_set.end());
            set_ref_aggregate(rel, related_idx, prods);
            return rel;
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void aggregate_unassign_object(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || products.empty()) return;

    try {
        const auto* rel_agg_decl = file->schema()->declaration_by_name("IfcRelAggregates");
        auto* rel_entity_decl = rel_agg_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (auto* product : products) {
            if (product) products_set.insert(const_cast<IfcUtil::IfcBaseClass*>(product));
        }

        // Collect all affected rels.
        std::set<IfcUtil::IfcBaseClass*> rels;
        for (auto* p : products_set) {
            auto* rel = find_decomposes(file, p);
            if (rel) rels.insert(rel);
        }

        for (auto* rel : rels) {
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
        }
    } catch (...) {}
}

IfcUtil::IfcBaseClass* spatial_assign_container(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* relating_structure,
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
        auto* structure = relating_structure;
        if (!structure) {
            set_error("Relating structure not found");
            return nullptr;
        }

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (auto* product : products) {
            if (product) products_set.insert(const_cast<IfcUtil::IfcBaseClass*>(product));
        }
        if (products_set.empty()) return nullptr;

        // Find existing container relationship on the structure.
        auto* existing_rel = find_contains_elements(file, structure);

        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelContainedInSpatialStructure");
        auto* rel_entity_decl = rel_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedElements");

        // Determine which products need to change.
        std::set<IfcUtil::IfcBaseClass*> previous_rels;
        std::vector<IfcUtil::IfcBaseClass*> products_to_change;

        for (auto* product : products_set) {
            auto* cur_rel = find_contained_in_structure(file, product);
            if (cur_rel == nullptr) {
                products_to_change.push_back(product);
            } else if (cur_rel != existing_rel) {
                previous_rels.insert(cur_rel);
                products_to_change.push_back(product);
            }
        }

        if (products_to_change.empty()) {
            return existing_rel;
        }

        // Unassign from aggregates (products can't be both aggregated and contained).
        for (auto* product : products_to_change) {
            auto* agg_rel = find_decomposes(file, product);
            if (!agg_rel) continue;
            const auto* agg_decl = file->schema()->declaration_by_name("IfcRelAggregates");
            auto* agg_entity_decl = agg_decl->as_entity();
            int agg_re_idx = find_attr_index(agg_entity_decl, "RelatedObjects");
            auto objs = get_ref_aggregate(agg_rel, agg_re_idx);
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* e : objs) {
                if (products_set.find(e) == products_set.end()) {
                    remaining.push_back(e);
                }
            }
            if (remaining.empty()) {
                remove_with_history(file, agg_rel);
            } else {
                set_ref_aggregate(agg_rel, agg_re_idx, remaining);
                update_owner_history(file, agg_rel, user, application);
            }
        }

        // Unassign from previous containers.
        for (auto* prev_rel : previous_rels) {
            auto elems = get_ref_aggregate(prev_rel, related_idx);
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* e : elems) {
                if (products_set.find(e) == products_set.end()) {
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

        // Add to target container.
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());
            for (auto* p : products_set) current_set.insert(p);
            std::vector<IfcUtil::IfcBaseClass*> merged(current_set.begin(), current_set.end());
            set_ref_aggregate(existing_rel, related_idx, merged);
            update_owner_history(file, existing_rel, user, application);
            return existing_rel;
        } else {
            auto* rel = file->create(rel_decl);
            if (!rel) {
                set_error("Failed to create IfcRelContainedInSpatialStructure");
                return nullptr;
            }
            int gi_idx = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int rs_idx = find_attr_index(rel_entity_decl, "RelatingStructure");
            set_ref(rel, rs_idx, structure);
            int oh_idx = find_attr_index(rel_entity_decl, "OwnerHistory");
            set_ref(rel, oh_idx, ensure_owner_history(file, owner_history, user, application));
            std::vector<IfcUtil::IfcBaseClass*> prods(products_set.begin(), products_set.end());
            set_ref_aggregate(rel, related_idx, prods);
            return rel;
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void spatial_unassign_container(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || products.empty()) return;

    try {
        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelContainedInSpatialStructure");
        auto* rel_entity_decl = rel_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedElements");

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (auto* product : products) {
            if (product) products_set.insert(const_cast<IfcUtil::IfcBaseClass*>(product));
        }

        std::set<IfcUtil::IfcBaseClass*> rels;
        for (auto* p : products_set) {
            auto* rel = find_contained_in_structure(file, p);
            if (rel) rels.insert(rel);
        }

        for (auto* rel : rels) {
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
        }
    } catch (...) {}
}

} // namespace bindings
} // namespace ifcapi
