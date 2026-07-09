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
    std::vector<express::Base> products;
    express::Base relating_structure;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for removing products from spatial containment.
 */
struct SpatialUnassignContainerOptions {
    std::vector<express::Base> products;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for referencing products in a spatial structure.
 */
struct SpatialReferenceStructureOptions {
    std::vector<express::Base> products;
    express::Base relating_structure;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for dereferencing products from a spatial structure.
 */
struct SpatialDereferenceStructureOptions {
    std::vector<express::Base> products;
    express::Base relating_structure;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Assign products to be contained hierarchically in a spatial structure.
 */
IFCAPI_BINDING express::Base spatial_assign_container(
    ifcopenshell::file* file,
    const SpatialAssignContainerOptions& options);

/**
 * Remove products from their spatial containment relationships.
 */
IFCAPI_BINDING void spatial_unassign_container(
    ifcopenshell::file* file,
    const SpatialUnassignContainerOptions& options);

/**
 * Reference products in a spatial structure without containment.
 */
IFCAPI_BINDING express::Base spatial_reference_structure(
    ifcopenshell::file* file,
    const SpatialReferenceStructureOptions& options);

/**
 * Dereference products from a spatial structure.
 */
IFCAPI_BINDING void spatial_dereference_structure(
    ifcopenshell::file* file,
    const SpatialDereferenceStructureOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SPATIAL_H */
