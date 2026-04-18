// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "guid.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

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

// Find the IfcRelDefinesByType that the given type is the RelatingType of.
// For IFC2X3, the inverse is "ObjectTypeOf"; for IFC4+, it is "Types".
static IfcUtil::IfcBaseClass* find_types_rel(IfcUtil::IfcBaseClass* relating_type) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(relating_type);
    if (!be) return nullptr;
    // Try IFC4+ "Types" first, then IFC2X3 "ObjectTypeOf"
    for (const char* inverse_name : {"Types", "ObjectTypeOf"}) {
        try {
            auto result = be->get_inverse(inverse_name);
            if (result && result->size() > 0) return (*result)[0];
        } catch (...) {}
    }
    return nullptr;
}

// Find the IfcRelDefinesByType that the element is typed by.
// For IFC4+: inverse "IsTypedBy"; for IFC2X3: filter "IsDefinedBy" for IfcRelDefinesByType.
static IfcUtil::IfcBaseClass* find_element_type_rel(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* element) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(element);
    if (!be) return nullptr;

    // IFC4+: IsTypedBy
    try {
        auto result = be->get_inverse("IsTypedBy");
        if (result && result->size() > 0) return (*result)[0];
    } catch (...) {}

    // IFC2X3: IsDefinedBy, filter for IfcRelDefinesByType
    try {
        auto result = be->get_inverse("IsDefinedBy");
        if (result) {
            const auto* rdt_decl = file->schema()->declaration_by_name("IfcRelDefinesByType");
            for (size_t i = 0; i < result->size(); ++i) {
                if ((*result)[i]->declaration().is(*rdt_decl)) {
                    return (*result)[i];
                }
            }
        }
    } catch (...) {}

    return nullptr;
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

extern "C" {

// Forward declarations of geometry helpers used during type assignment.
bool ifcopenshell_type_map_type_representations(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* related_object_h,
    ifcopenshell_ifc_instance_t* relating_type_h);

namespace {
// Strip ObjectType / PredefinedType from related objects when relating
// type already declares a non-NOTDEFINED PredefinedType, mirroring the
// upstream Python guard against "double typing" (issue 7006).
void clear_predefined_type_on_objects(
    const std::set<IfcUtil::IfcBaseClass*>& objects_set,
    IfcUtil::IfcBaseClass* relating_type)
{
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(relating_type);
    if (!be) return;
    auto* d = be->declaration().as_entity();
    if (!d) return;
    int pdt_idx = d->attribute_index("PredefinedType");
    if (pdt_idx < 0) return;
    std::string predefined;
    try {
        auto v = relating_type->get_attribute_value(static_cast<size_t>(pdt_idx));
        if (v.isNull()) return;
        predefined = (std::string)v;
    } catch (...) { return; }
    if (predefined.empty() || predefined == "NOTDEFINED") return;

    for (auto* obj : objects_set) {
        auto* obe = dynamic_cast<IfcUtil::IfcBaseEntity*>(obj);
        if (!obe) continue;
        auto* od = obe->declaration().as_entity();
        if (!od) continue;
        int ot_idx = od->attribute_index("ObjectType");
        if (ot_idx >= 0) {
            try { obj->unset_attribute_value(static_cast<size_t>(ot_idx)); } catch (...) {}
        }
        int p_idx = od->attribute_index("PredefinedType");
        if (p_idx >= 0) {
            try { obj->unset_attribute_value(static_cast<size_t>(p_idx)); } catch (...) {}
        }
    }
}

// Core implementation shared by ifcopenshell_type_assign_type and
// ifcopenshell_type_assign_type_ex. When should_map_representations is
// true we additionally propagate the relating type's IfcRepresentationMaps
// onto each newly-assigned related object and clear any redundant
// ObjectType / PredefinedType (matching upstream Python behaviour).
ifcopenshell_ifc_instance_t* assign_type_core(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_type,
    bool should_map_representations);
}

ifcopenshell_ifc_instance_t* ifcopenshell_type_assign_type(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_type)
{
    return assign_type_core(file_ptr, objects, object_count, relating_type, true);
}

ifcopenshell_ifc_instance_t* ifcopenshell_type_assign_type_ex(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_type,
    bool should_map_representations)
{
    return assign_type_core(file_ptr, objects, object_count, relating_type,
                            should_map_representations);
}

namespace {
ifcopenshell_ifc_instance_t* assign_type_core(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_type,
    bool should_map_representations)
{
    ifcopenshell_clear_error();
    if (!file_ptr || !objects || object_count == 0 || false) {
        set_error("Invalid arguments");
        return 0;
    }

    try {
        auto* file = file_ptr->ptr;

        auto* relating_type_e = relating_type ? relating_type->ptr : nullptr;
        if (!relating_type_e) {
            set_error("Relating type not found");
            return 0;
        }

        std::set<IfcUtil::IfcBaseClass*> objects_set;
        for (uint32_t i = 0; i < object_count; ++i) {
            auto* obj = (objects[i] ? objects[i]->ptr : nullptr);
            if (obj) objects_set.insert(obj);
        }
        if (objects_set.empty()) return 0;

        auto* existing_rel = find_types_rel(relating_type_e);

        const auto* rdt_decl = file->schema()->declaration_by_name("IfcRelDefinesByType");
        auto* rdt_entity_decl = rdt_decl->as_entity();
        int related_idx = find_attr_index(rdt_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> previous_rels;
        std::vector<IfcUtil::IfcBaseClass*> objects_to_change;

        for (auto* obj : objects_set) {
            auto* cur_rel = find_element_type_rel(file, obj);
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

        // Remove from previous type relationships.
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

        IfcUtil::IfcBaseClass* result_rel = nullptr;

        // Add to target type relationship.
        if (existing_rel) {
            auto current = get_ref_aggregate(existing_rel, related_idx);
            std::set<IfcUtil::IfcBaseClass*> current_set(current.begin(), current.end());
            for (auto* o : objects_set) current_set.insert(o);
            std::vector<IfcUtil::IfcBaseClass*> merged(current_set.begin(), current_set.end());
            set_ref_aggregate(existing_rel, related_idx, merged);
            result_rel = existing_rel;
        } else {
            auto* rel = file->create(rdt_decl);
            if (!rel) {
                set_error("Failed to create IfcRelDefinesByType");
                return 0;
            }
            int gi_idx = find_attr_index(rdt_entity_decl, "GlobalId");
            if (gi_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(gi_idx), ifcapi::guid_new());
            }
            int rt_idx = find_attr_index(rdt_entity_decl, "RelatingType");
            if (rt_idx >= 0) {
                rel->set_attribute_value(static_cast<size_t>(rt_idx), relating_type_e);
            }
            std::vector<IfcUtil::IfcBaseClass*> objs(objects_set.begin(), objects_set.end());
            set_ref_aggregate(rel, related_idx, objs);
            result_rel = rel;
        }

        if (should_map_representations) {
            // Propagate IfcRepresentationMaps onto each newly-assigned object.
            auto* rt_be = dynamic_cast<IfcUtil::IfcBaseEntity*>(relating_type_e);
            if (rt_be) {
                int rm_idx = rt_be->declaration().as_entity()->attribute_index("RepresentationMaps");
                bool has_maps = false;
                if (rm_idx >= 0) {
                    try {
                        auto v = relating_type_e->get_attribute_value(static_cast<size_t>(rm_idx));
                        if (!v.isNull()) {
                            auto agg = (aggregate_of_instance::ptr)v;
                            has_maps = agg && agg->size() > 0;
                        }
                    } catch (...) {}
                }
                if (has_maps) {
                    auto* rt_h = ifcopenshell::capi::wrap_instance(relating_type_e);
                    for (auto* obj : objects_to_change) {
                        auto* obj_h = ifcopenshell::capi::wrap_instance(obj);
                        ifcopenshell_type_map_type_representations(file_ptr, obj_h, rt_h);
                        ifcopenshell_ifc_instance_destroy(obj_h);
                    }
                    ifcopenshell_ifc_instance_destroy(rt_h);
                }
            }

            clear_predefined_type_on_objects(objects_set, relating_type_e);
        }

        return ifcopenshell::capi::wrap_instance(result_rel);
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    }
}
}  // namespace

void ifcopenshell_type_unassign_type(
    ifcopenshell_ifc_file_t* file_ptr,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count)
{
    if (!file_ptr || !objects || object_count == 0) return;

    try {
        auto* file = file_ptr->ptr;

        const auto* rdt_decl = file->schema()->declaration_by_name("IfcRelDefinesByType");
        auto* rdt_entity_decl = rdt_decl->as_entity();
        int related_idx = find_attr_index(rdt_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> objects_set;
        for (uint32_t i = 0; i < object_count; ++i) {
            auto* obj = (objects[i] ? objects[i]->ptr : nullptr);
            if (obj) objects_set.insert(obj);
        }

        std::set<IfcUtil::IfcBaseClass*> rels;
        for (auto* obj : objects_set) {
            auto* rel = find_element_type_rel(file, obj);
            if (rel) rels.insert(rel);
        }

        for (auto* rel : rels) {
            auto related = get_ref_aggregate(rel, related_idx);
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
