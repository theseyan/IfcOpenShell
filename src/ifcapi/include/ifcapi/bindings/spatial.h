/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SPATIAL_H
#define IFCAPI_BINDINGS_SPATIAL_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning products to be contained in a spatial structure.
 */
struct SpatialAssignContainerOptions {
    /// Products to contain in the spatial structure.
    std::vector<express::Base> products;
    /// The IfcSpatialStructureElement (e.g. IfcBuildingStorey, IfcSpace) to contain the products.
    express::Base relating_structure;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing products from spatial containment.
 */
struct SpatialUnassignContainerOptions {
    /// Products to remove from their spatial containment relationships.
    std::vector<express::Base> products;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Options for referencing products in a spatial structure.
 */
struct SpatialReferenceStructureOptions {
    /// Products to reference in the spatial structure.
    std::vector<express::Base> products;
    /// The IfcSpatialStructureElement to reference the products from.
    express::Base relating_structure;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for dereferencing products from a spatial structure.
 */
struct SpatialDereferenceStructureOptions {
    /// Products to remove from the spatial reference.
    std::vector<express::Base> products;
    /// The IfcSpatialStructureElement to dereference from.
    express::Base relating_structure;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Contain products in a spatial structure via IfcRelContainedInSpatialStructure.
 *
 * Products previously contained in a different structure are moved. Products
 * previously aggregated under a different parent (IfcRelAggregates) are
 * removed from that aggregation. If an existing containment relationship
 * already relates to the same structure, products are merged into it.
 */
IFCAPI_BINDING express::Base spatial_assign_container(
    ifcopenshell::file* file,
    const SpatialAssignContainerOptions& options);

/**
 * Remove products from their IfcRelContainedInSpatialStructure relationships.
 *
 * If no related elements remain after removal, the relationship is deleted.
 */
IFCAPI_BINDING void spatial_unassign_container(
    ifcopenshell::file* file,
    const SpatialUnassignContainerOptions& options);

/**
 * Reference products in a spatial structure via IfcRelReferencedInSpatialStructure.
 *
 * Unlike containment, referencing does not establish ownership. Products
 * already referenced by the same structure are skipped.
 */
IFCAPI_BINDING express::Base spatial_reference_structure(
    ifcopenshell::file* file,
    const SpatialReferenceStructureOptions& options);

/**
 * Remove products from a spatial structure reference relationship.
 *
 * Removes products from IfcRelReferencedInSpatialStructure relationships
 * for the given structure. If no products remain, the relationship is deleted.
 */
IFCAPI_BINDING void spatial_dereference_structure(
    ifcopenshell::file* file,
    const SpatialDereferenceStructureOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SPATIAL_H */
