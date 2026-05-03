// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_RELATIONSHIP_H
#define IFCAPI_DETAIL_RELATIONSHIP_H

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/owner.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <vector>

namespace ifcapi {
namespace detail {

inline int find_attr_index(const IfcParse::entity* decl, const char* name) {
    if (!decl) {
        return -1;
    }
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline std::vector<IfcUtil::IfcBaseClass*> get_ref_aggregate(IfcUtil::IfcBaseClass* entity, int attr_idx) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!entity || attr_idx < 0) {
        return result;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(attr_idx));
        if (val.isNull()) {
            return result;
        }
        auto agg = (aggregate_of_instance::ptr)val;
        if (agg) {
            for (auto& item : *agg) {
                result.push_back(item);
            }
        }
    } catch (...) {
    }
    return result;
}

inline void set_ref_aggregate(
    IfcUtil::IfcBaseClass* entity,
    int attr_idx,
    const std::vector<IfcUtil::IfcBaseClass*>& refs)
{
    if (!entity || attr_idx < 0) {
        return;
    }
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* ref : refs) {
        agg->push(ref);
    }
    entity->set_attribute_value(static_cast<size_t>(attr_idx), agg);
}

inline void set_ref(IfcUtil::IfcBaseClass* entity, int attr_idx, IfcUtil::IfcBaseClass* ref) {
    if (entity && attr_idx >= 0 && ref) {
        entity->set_attribute_value(static_cast<size_t>(attr_idx), ref);
    }
}

inline IfcUtil::IfcBaseClass* ensure_owner_history(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    return owner_history ? owner_history : ifcapi::bindings::owner_create_owner_history(file, user, application);
}

inline void update_owner_history(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* entity,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (entity && user && application) {
        ifcapi::bindings::owner_update_owner_history(file, entity, user, application);
    }
}

inline void remove_with_history(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity) {
        return;
    }
    auto* decl = entity->declaration().as_entity();
    int owner_history_idx = find_attr_index(decl, "OwnerHistory");
    IfcUtil::IfcBaseClass* history = nullptr;
    if (owner_history_idx >= 0) {
        try {
            auto val = entity->get_attribute_value(static_cast<size_t>(owner_history_idx));
            if (!val.isNull()) {
                history = (IfcUtil::IfcBaseClass*)val;
            }
        } catch (...) {
        }
    }
    file->removeEntity(entity);
    if (history) {
        ifcapi::bindings::entity_remove_deep2(history);
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_RELATIONSHIP_H */
