/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_AGGREGATE_H
#define IFCAPI_BINDINGS_AGGREGATE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning products as parts of an aggregate.
 */
struct AggregateAssignObjectOptions {
    std::vector<express::Base> products;
    express::Base relating_object;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for removing products from aggregate relationships.
 */
struct AggregateUnassignObjectOptions {
    std::vector<express::Base> products;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Assign products as parts of an aggregate object.
 */
IFCAPI_BINDING express::Base aggregate_assign_object(
    ifcopenshell::file* file,
    const AggregateAssignObjectOptions& options);

/**
 * Remove products from their aggregate relationships.
 */
IFCAPI_BINDING void aggregate_unassign_object(ifcopenshell::file* file, const AggregateUnassignObjectOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_AGGREGATE_H */
