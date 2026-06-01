// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_RELATIONSHIP_H
#define IFCAPI_DETAIL_RELATIONSHIP_H

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/owner.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/file.h"

namespace ifcapi {
namespace detail {

inline express::Base* nullable_ptr(express::Base& value) {
    return value ? &value : nullptr;
}

inline bool same_instance(express::Base left, express::Base right) {
    if (!left || !right) {
        return !left && !right;
    }
    try {
        return left.file() == right.file() && left.id() == right.id();
    } catch (...) {
        return false;
    }
}

inline express::Base ensure_owner_history(
    ifcopenshell::file* file,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    return owner_history
        ? owner_history
        : ifcapi::bindings::owner_create_owner_history(file, nullable_ptr(user), nullable_ptr(application));
}

inline void update_owner_history(
    ifcopenshell::file* file,
    express::Base entity,
    express::Base user,
    express::Base application)
{
    if (entity && user && application) {
        ifcapi::bindings::owner_update_owner_history(file, &entity, &user, &application);
    }
}

inline void remove_with_history(ifcopenshell::file* file, express::Base entity) {
    if (!file || !entity) {
        return;
    }
    auto history = read_ref_attr(entity, "OwnerHistory");
    file->remove_entity(entity);
    if (history) {
        ifcapi::bindings::entity_remove_deep2(&history);
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_RELATIONSHIP_H */
