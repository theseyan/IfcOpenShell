// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "guid.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

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

static int find_attr_index(const IfcParse::entity* decl, const char* name) {
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) return static_cast<int>(i);
    }
    return -1;
}

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

static void set_ref_aggregate(IfcUtil::IfcBaseClass* entity, int attr_idx,
                              const std::vector<IfcUtil::IfcBaseClass*>& refs) {
    if (attr_idx < 0) return;
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* ref : refs) {
        agg->push(ref);
    }
    entity->set_attribute_value(static_cast<size_t>(attr_idx), agg);
}

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

extern "C" {

ifcopenshell_ifc_instance_t* ifcopenshell_group_assign_group(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* group)
{
    ifcopenshell_clear_error();
    if (!file_ptr || !products || product_count == 0 || false) {
        set_error("Invalid arguments");
        return 0;
    }

    try {
        auto* file = file_ptr->ptr;

        auto* group_e = group ? group->ptr : nullptr;
        if (!group_e) {
            set_error("Group not found");
            return 0;
        }

        std::vector<IfcUtil::IfcBaseClass*> products_vec;
        for (uint32_t i = 0; i < product_count; ++i) {
            auto* obj = (products[i] ? products[i]->ptr : nullptr);
            if (obj) products_vec.push_back(obj);
        }
        if (products_vec.empty()) return 0;

        auto* existing_rel = find_is_grouped_by(group_e);

        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelAssignsToGroup");
        auto* rel_entity_decl = rel_decl->as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        if (!existing_rel) {
            auto* rel = file->create(rel_decl);
            if (!rel) return 0;
            int gi_idx = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int rg_idx = find_attr_index(rel_entity_decl, "RelatingGroup");
            if (rg_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(rg_idx), group_e);
            }
            set_ref_aggregate(rel, related_idx, products_vec);
            return ifcopenshell::capi::wrap_instance(rel);
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
            return ifcopenshell::capi::wrap_instance(existing_rel);
        }

        for (auto* p : products_vec) {
            if (current_set.insert(p).second) {
                current.push_back(p);
            }
        }
        set_ref_aggregate(existing_rel, related_idx, current);
        return ifcopenshell::capi::wrap_instance(existing_rel);
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    }
}

void ifcopenshell_group_unassign_group(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* group)
{
    if (!file_ptr || !products || product_count == 0 || !group) return;

    try {
        auto* file = file_ptr->ptr;

        auto* group_e = group ? group->ptr : nullptr;
        if (!group_e) return;

        auto* rel = find_is_grouped_by(group_e);
        if (!rel) return;

        auto* rel_entity_decl = rel->declaration().as_entity();
        int related_idx = find_attr_index(rel_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> products_set;
        for (uint32_t i = 0; i < product_count; ++i) {
            auto* obj = (products[i] ? products[i]->ptr : nullptr);
            if (obj) products_set.insert(obj);
        }

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
    } catch (...) {}
}

} // extern "C"
