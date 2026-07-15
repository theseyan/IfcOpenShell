/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_NEST_H
#define IFCAPI_BINDINGS_NEST_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning objects as nested children of a parent host.
 */
struct NestAssignObjectOptions {
    /// Objects to nest under the relating object.
    std::vector<express::Base> products;
    /// The parent object that will own the nested children.
    express::Base relating_object;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing objects from their nesting relationships.
 */
struct NestUnassignObjectOptions {
    /// Objects to remove from their current nesting relationships.
    std::vector<express::Base> products;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

struct NestChangeNestOptions {
    express::Base item;
    express::Base new_parent;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/** Move an already nested child to a new parent, appending it after the target parent's current children. */
IFCAPI_BINDING void nest_change_nest(ifcopenshell::file* file, const NestChangeNestOptions& options);

struct NestReorderNestingOptions {
    express::Base item;
    std::optional<int> old_index;
    std::optional<int> new_index = 0;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/** Reorder an existing nested child with Python-compatible index semantics; omitted old_index locates item. */
IFCAPI_BINDING void nest_reorder_nesting(ifcopenshell::file* file, const NestReorderNestingOptions& options);

/**
 * Assign objects as nested children of a parent host via IfcRelNests.
 *
 * Objects previously nested under a different parent are moved. Objects
 * previously contained in a spatial structure or aggregated under a
 * different parent are removed from those relationships. If an existing
 * IfcRelNests relationship already relates to the same parent, objects
 * are merged into it while preserving insertion order.
 */
IFCAPI_BINDING express::Base nest_assign_object(
    ifcopenshell::file* file,
    const NestAssignObjectOptions& options);

/**
 * Remove objects from their IfcRelNests relationships.
 *
 * If no related objects remain after removal, the IfcRelNests
 * relationship itself is deleted.
 */
IFCAPI_BINDING void nest_unassign_object(ifcopenshell::file* file, const NestUnassignObjectOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_NEST_H */
