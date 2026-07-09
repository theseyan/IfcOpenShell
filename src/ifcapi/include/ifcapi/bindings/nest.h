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
    std::vector<express::Base> products;
    express::Base relating_object;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for removing objects from their nesting relationships.
 */
struct NestUnassignObjectOptions {
    std::vector<express::Base> products;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Assign objects as nested children of a parent host.
 */
IFCAPI_BINDING express::Base nest_assign_object(
    ifcopenshell::file* file,
    const NestAssignObjectOptions& options);

/**
 * Remove objects from their nesting relationships.
 */
IFCAPI_BINDING void nest_unassign_object(ifcopenshell::file* file, const NestUnassignObjectOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_NEST_H */
