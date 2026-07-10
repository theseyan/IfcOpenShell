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
    /// Products to assign as parts of the relating object.
    std::vector<express::Base> products;
    /// The parent object that will own the aggregate (e.g. an IfcSite, IfcBuilding, or IfcElementAssembly).
    express::Base relating_object;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing products from aggregate relationships.
 */
struct AggregateUnassignObjectOptions {
    /// Products to remove from their current aggregate relationships.
    std::vector<express::Base> products;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Assign products as parts of an aggregate object via IfcRelAggregates.
 *
 * Products already aggregated under a different parent are moved. Products
 * previously contained in a spatial structure (IfcRelContainedInSpatialStructure)
 * are removed from that containment. If an existing IfcRelAggregates
 * relationship already relates to the same parent, products are merged into it.
 */
IFCAPI_BINDING express::Base aggregate_assign_object(
    ifcopenshell::file* file,
    const AggregateAssignObjectOptions& options);

/**
 * Remove products from their IfcRelAggregates relationships.
 *
 * If no related objects remain after removal, the IfcRelAggregates
 * relationship itself is deleted.
 */
IFCAPI_BINDING void aggregate_unassign_object(ifcopenshell::file* file, const AggregateUnassignObjectOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_AGGREGATE_H */
