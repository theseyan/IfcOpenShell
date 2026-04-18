// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
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

// Helper: find attribute index by name on an entity declaration, returns -1 if not found.
static int find_attr_index(const IfcParse::entity* decl, const char* name) {
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) return static_cast<int>(i);
    }
    return -1;
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

// Helper: get aggregate of entity references from an attribute.
static std::vector<IfcUtil::IfcBaseClass*> get_ref_aggregate(IfcUtil::IfcBaseClass* entity, int attr_idx) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (attr_idx < 0) return result;
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(attr_idx));
        if (val.isNull()) return result;
        auto agg = (aggregate_of_instance::ptr)val;
        if (agg) {
            for (auto& item : *agg) {
                result.push_back(item);
            }
        }
    } catch (...) {}
    return result;
}

// Helper: set aggregate of entity references on an attribute.
static void set_ref_aggregate(IfcUtil::IfcBaseClass* entity, int attr_idx,
                              const std::vector<IfcUtil::IfcBaseClass*>& refs) {
    if (attr_idx < 0) return;
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* ref : refs) {
        agg->push(ref);
    }
    entity->set_attribute_value(static_cast<size_t>(attr_idx), agg);
}

// Helper: remove an entity and its OwnerHistory if orphaned.
static void remove_with_history(ifcopenshell_ifc_file_t* file, IfcUtil::IfcBaseClass* entity) {
    auto* decl = entity->declaration().as_entity();
    int oh_idx = decl ? find_attr_index(decl, "OwnerHistory") : -1;
    IfcUtil::IfcBaseClass* history = nullptr;
    if (oh_idx >= 0) {
        try {
            auto val = entity->get_attribute_value(static_cast<size_t>(oh_idx));
            if (!val.isNull()) {
                history = (IfcUtil::IfcBaseClass*)val;
            }
        } catch (...) {}
    }
    file->ptr->removeEntity(entity);
    if (history) {
        { auto* _h = ifcopenshell::capi::wrap_instance(history); ifcopenshell_util_remove_deep2(_h); ifcopenshell_ifc_instance_destroy(_h); }
    }
}

extern "C" {

ifcopenshell_ifc_instance_t* ifcopenshell_aggregate_assign_object(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* relating_object)
{
    ifcopenshell_clear_error();
    if (!file_ptr || !products || product_count == 0 || false) {
        set_error("Invalid arguments");
        return 0;
    }

    try {
        auto* file = file_ptr->ptr;

        auto* relating = (relating_object ? relating_object->ptr : nullptr);
        if (!relating) {
            set_error("Relating object not found");
            return 0;
        }

        // Collect product entities.
        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (uint32_t i = 0; i < product_count; ++i) {
            auto* p = (products[i] ? products[i]->ptr : nullptr);
            if (p) products_set.insert(p);
        }
        if (products_set.empty()) return 0;

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
            return ifcopenshell::capi::wrap_instance(existing_rel);
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
                remove_with_history(file_ptr, container_rel);
            } else {
                set_ref_aggregate(container_rel, re_idx, remaining);
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
                remove_with_history(file_ptr, prev_rel);
            } else {
                set_ref_aggregate(prev_rel, related_idx, remaining);
            }
        }

        // Add to target aggregate.
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());
            for (auto* p : products_set) current_set.insert(p);
            std::vector<IfcUtil::IfcBaseClass*> merged(current_set.begin(), current_set.end());
            set_ref_aggregate(existing_rel, related_idx, merged);
            return ifcopenshell::capi::wrap_instance(existing_rel);
        } else {
            // Create new IfcRelAggregates.
            auto* rel = file->create(rel_agg_decl);
            if (!rel) {
                set_error("Failed to create IfcRelAggregates");
                return 0;
            }
            int gi_idx = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int ro_idx = find_attr_index(rel_entity_decl, "RelatingObject");
            if (ro_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(ro_idx), relating);
            }
            std::vector<IfcUtil::IfcBaseClass*> prods(products_set.begin(), products_set.end());
            set_ref_aggregate(rel, related_idx, prods);
            return ifcopenshell::capi::wrap_instance(rel);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    }
}

void ifcopenshell_aggregate_unassign_object(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count)
{
    if (!file_ptr || !products || product_count == 0) return;

    try {
        auto* file = file_ptr->ptr;
        const auto* rel_agg_decl = file->schema()->declaration_by_name("IfcRelAggregates");
        auto* rel_entity_decl = rel_agg_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (uint32_t i = 0; i < product_count; ++i) {
            auto* p = (products[i] ? products[i]->ptr : nullptr);
            if (p) products_set.insert(p);
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
                remove_with_history(file_ptr, rel);
            } else {
                set_ref_aggregate(rel, related_idx, remaining);
            }
        }
    } catch (...) {}
}

ifcopenshell_ifc_instance_t* ifcopenshell_spatial_assign_container(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* relating_structure)
{
    ifcopenshell_clear_error();
    if (!file_ptr || !products || product_count == 0 || false) {
        set_error("Invalid arguments");
        return 0;
    }

    try {
        auto* file = file_ptr->ptr;

        auto* structure = (relating_structure ? relating_structure->ptr : nullptr);
        if (!structure) {
            set_error("Relating structure not found");
            return 0;
        }

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (uint32_t i = 0; i < product_count; ++i) {
            auto* p = (products[i] ? products[i]->ptr : nullptr);
            if (p) products_set.insert(p);
        }
        if (products_set.empty()) return 0;

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
            return ifcopenshell::capi::wrap_instance(existing_rel);
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
                remove_with_history(file_ptr, agg_rel);
            } else {
                set_ref_aggregate(agg_rel, agg_re_idx, remaining);
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
                remove_with_history(file_ptr, prev_rel);
            } else {
                set_ref_aggregate(prev_rel, related_idx, remaining);
            }
        }

        // Add to target container.
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());
            for (auto* p : products_set) current_set.insert(p);
            std::vector<IfcUtil::IfcBaseClass*> merged(current_set.begin(), current_set.end());
            set_ref_aggregate(existing_rel, related_idx, merged);
            return ifcopenshell::capi::wrap_instance(existing_rel);
        } else {
            auto* rel = file->create(rel_decl);
            if (!rel) {
                set_error("Failed to create IfcRelContainedInSpatialStructure");
                return 0;
            }
            int gi_idx = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int rs_idx = find_attr_index(rel_entity_decl, "RelatingStructure");
            if (rs_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(rs_idx), structure);
            }
            std::vector<IfcUtil::IfcBaseClass*> prods(products_set.begin(), products_set.end());
            set_ref_aggregate(rel, related_idx, prods);
            return ifcopenshell::capi::wrap_instance(rel);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    }
}

void ifcopenshell_spatial_unassign_container(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count)
{
    if (!file_ptr || !products || product_count == 0) return;

    try {
        auto* file = file_ptr->ptr;
        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelContainedInSpatialStructure");
        auto* rel_entity_decl = rel_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedElements");

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (uint32_t i = 0; i < product_count; ++i) {
            auto* p = (products[i] ? products[i]->ptr : nullptr);
            if (p) products_set.insert(p);
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
                remove_with_history(file_ptr, rel);
            } else {
                set_ref_aggregate(rel, related_idx, remaining);
            }
        }
    } catch (...) {}
}

} // extern "C"
