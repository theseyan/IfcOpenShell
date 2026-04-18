// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "guid.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

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

// Find IfcRelNests where this entity is the RelatingObject (the whole).
// IFC4+: inverse "IsNestedBy"; IFC2X3: filter "IsDecomposedBy" for IfcRelNests.
static IfcUtil::IfcBaseClass* find_is_nested_by(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!be) return nullptr;
    // IFC4+
    try {
        auto result = be->get_inverse("IsNestedBy");
        if (result && result->size() > 0) return (*result)[0];
    } catch (...) {}
    // IFC2X3: filter IsDecomposedBy for IfcRelNests
    try {
        auto result = be->get_inverse("IsDecomposedBy");
        if (result) {
            const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
            for (size_t i = 0; i < result->size(); ++i) {
                if ((*result)[i]->declaration().is(*nests_decl)) {
                    return (*result)[i];
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

// Find IfcRelNests where this entity is a nested child.
// IFC4+: inverse "Nests"; IFC2X3: filter "Decomposes" for IfcRelNests.
static IfcUtil::IfcBaseClass* find_nests(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!be) return nullptr;
    // IFC4+
    try {
        auto result = be->get_inverse("Nests");
        if (result && result->size() > 0) return (*result)[0];
    } catch (...) {}
    // IFC2X3: filter Decomposes for IfcRelNests
    try {
        auto result = be->get_inverse("Decomposes");
        if (result) {
            const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
            for (size_t i = 0; i < result->size(); ++i) {
                if ((*result)[i]->declaration().is(*nests_decl)) {
                    return (*result)[i];
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

extern "C" {

ifcopenshell_ifc_instance_t* ifcopenshell_nest_assign_object(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_object)
{
    ifcopenshell_clear_error();
    if (!file_ptr || !objects || object_count == 0 || false) {
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

        // Maintain insertion order (nesting order matters in IFC).
        std::vector<IfcUtil::IfcBaseClass*> objects_vec;
        std::set<IfcUtil::IfcBaseClass*> objects_set;
        for (uint32_t i = 0; i < object_count; ++i) {
            auto* obj = (objects[i] ? objects[i]->ptr : nullptr);
            if (obj && objects_set.insert(obj).second) {
                objects_vec.push_back(obj);
            }
        }
        if (objects_vec.empty()) return 0;

        auto* existing_rel = find_is_nested_by(file, relating);

        const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
        auto* nests_entity_decl = nests_decl->as_entity();
        int related_idx = find_attr_index(nests_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> previous_rels;
        std::vector<IfcUtil::IfcBaseClass*> objects_to_change;

        for (auto* obj : objects_vec) {
            auto* cur_rel = find_nests(file, obj);
            if (cur_rel == nullptr) {
                objects_to_change.push_back(obj);
            } else if (cur_rel != existing_rel) {
                previous_rels.insert(cur_rel);
                objects_to_change.push_back(obj);
            }
        }

        if (objects_to_change.empty()) {
            return ifcopenshell::capi::wrap_instance(existing_rel);
        }

        // Unassign from spatial containers.
        {
            std::vector<ifcopenshell_ifc_instance_t*> container_handles;
            for (auto* obj : objects_to_change) {
                auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(obj);
                if (!be) continue;
                try {
                    auto inv = be->get_inverse("ContainedInStructure");
                    if (inv && inv->size() > 0) {
                        container_handles.push_back(ifcopenshell::capi::wrap_instance(obj));
                    }
                } catch (...) {}
            }
            if (!container_handles.empty()) {
                ifcopenshell_spatial_unassign_container(file_ptr, container_handles.data(),
                                                  static_cast<uint32_t>(container_handles.size()));
            }
            for (auto* h : container_handles) ifcopenshell_ifc_instance_destroy(h);
        }

        // Unassign from aggregates.
        {
            std::vector<ifcopenshell_ifc_instance_t*> agg_handles;
            for (auto* obj : objects_to_change) {
                agg_handles.push_back(ifcopenshell::capi::wrap_instance(obj));
            }
            ifcopenshell_aggregate_unassign_object(file_ptr, agg_handles.data(),
                                             static_cast<uint32_t>(agg_handles.size()));
            for (auto* h : agg_handles) ifcopenshell_ifc_instance_destroy(h);
        }

        // Remove from previous nest rels (preserving order).
        for (auto* prev_rel : previous_rels) {
            auto related = get_ref_aggregate(prev_rel, related_idx);
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* e : related) {
                if (objects_set.find(e) == objects_set.end()) {
                    remaining.push_back(e);
                }
            }
            if (remaining.empty()) {
                remove_with_history(file_ptr, prev_rel);
            } else {
                set_ref_aggregate(prev_rel, related_idx, remaining);
            }
        }

        // Add to target nesting (preserving order).
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());
            for (auto* o : objects_vec) {
                if (current_set.insert(o).second) {
                    current.push_back(o);
                }
            }
            set_ref_aggregate(existing_rel, related_idx, current);
            return ifcopenshell::capi::wrap_instance(existing_rel);
        } else {
            auto* rel = file->create(nests_decl);
            if (!rel) {
                set_error("Failed to create IfcRelNests");
                return 0;
            }
            int gi_idx = find_attr_index(nests_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int ro_idx = find_attr_index(nests_entity_decl, "RelatingObject");
            if (ro_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(ro_idx), relating);
            }
            set_ref_aggregate(rel, related_idx, objects_vec);
            return ifcopenshell::capi::wrap_instance(rel);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    }
}

void ifcopenshell_nest_unassign_object(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count)
{
    if (!file_ptr || !objects || object_count == 0) return;

    try {
        auto* file = file_ptr->ptr;

        const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
        auto* nests_entity_decl = nests_decl->as_entity();
        int related_idx = find_attr_index(nests_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> objects_set;
        for (uint32_t i = 0; i < object_count; ++i) {
            auto* obj = (objects[i] ? objects[i]->ptr : nullptr);
            if (obj) objects_set.insert(obj);
        }

        std::set<IfcUtil::IfcBaseClass*> rels;
        for (auto* obj : objects_set) {
            auto* rel = find_nests(file, obj);
            if (rel) rels.insert(rel);
        }

        for (auto* rel : rels) {
            auto related = get_ref_aggregate(rel, related_idx);
            // Preserve order while removing
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* e : related) {
                if (objects_set.find(e) == objects_set.end()) {
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
