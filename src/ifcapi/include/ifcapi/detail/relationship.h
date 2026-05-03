// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_RELATIONSHIP_H
#define IFCAPI_DETAIL_RELATIONSHIP_H

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/owner.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/IfcFile.h"

namespace ifcapi {
namespace detail {

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
