// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/spatial.h"
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

static void remove_with_history(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
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
    file->removeEntity(entity);
    if (history) {
        ifcapi::bindings::entity_remove_deep2(history);
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

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* nest_assign_object(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& objects,
    IfcUtil::IfcBaseClass* relating_object)
{
    ifcopenshell_clear_error();
    if (!file || objects.empty()) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        auto* relating = relating_object;
        if (!relating) {
            set_error("Relating object not found");
            return nullptr;
        }

        // Maintain insertion order (nesting order matters in IFC).
        std::vector<IfcUtil::IfcBaseClass*> objects_vec;
        std::set<IfcUtil::IfcBaseClass*> objects_set;
        for (auto* object : objects) {
            auto* obj = const_cast<IfcUtil::IfcBaseClass*>(object);
            if (obj && objects_set.insert(obj).second) {
                objects_vec.push_back(obj);
            }
        }
        if (objects_vec.empty()) return nullptr;

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
            return existing_rel;
        }

        // Unassign from spatial containers.
        {
            std::vector<const IfcUtil::IfcBaseClass*> contained_objects;
            for (auto* obj : objects_to_change) {
                auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(obj);
                if (!be) continue;
                try {
                    auto inv = be->get_inverse("ContainedInStructure");
                    if (inv && inv->size() > 0) {
                        contained_objects.push_back(obj);
                    }
                } catch (...) {}
            }
            if (!contained_objects.empty()) {
                spatial_unassign_container(file, contained_objects);
            }
        }

        // Unassign from aggregates.
        {
            std::vector<const IfcUtil::IfcBaseClass*> aggregate_objects(
                objects_to_change.begin(), objects_to_change.end());
            aggregate_unassign_object(file, aggregate_objects);
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
                remove_with_history(file, prev_rel);
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
            return existing_rel;
        } else {
            auto* rel = file->create(nests_decl);
            if (!rel) {
                set_error("Failed to create IfcRelNests");
                return nullptr;
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
            return rel;
        }
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void nest_unassign_object(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& objects)
{
    if (!file || objects.empty()) return;

    try {
        const auto* nests_decl = file->schema()->declaration_by_name("IfcRelNests");
        auto* nests_entity_decl = nests_decl->as_entity();
        int related_idx = find_attr_index(nests_entity_decl, "RelatedObjects");

        std::set<IfcUtil::IfcBaseClass*> objects_set;
        for (auto* object : objects) {
            auto* obj = const_cast<IfcUtil::IfcBaseClass*>(object);
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
                remove_with_history(file, rel);
            } else {
                set_ref_aggregate(rel, related_idx, remaining);
            }
        }
    } catch (...) {}
}

} // namespace bindings
} // namespace ifcapi
