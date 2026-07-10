/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ROOT_H
#define IFCAPI_BINDINGS_ROOT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

namespace ifcapi {
namespace bindings {

/**
 * Options for creating an IFC rooted entity.
 */
struct RootCreateEntityOptions {
    /// IFC entity class to instantiate, for example `IfcWall` or `IfcProject`.
    std::string ifc_class;
    /// Optional predefined type. Unsupported enum values are stored as user-defined type labels when possible.
    std::optional<std::string> predefined_type;
    /// Optional entity name.
    std::optional<std::string> name;
    /// Optional owner history to assign when the target schema supports it.
    std::optional<express::Base> owner_history;
};

/**
 * Options for removing a product.
 */
struct RootRemoveProductOptions {
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Create an IFC entity with generated identity, optional name, and optional
 * predefined type.
 *
 * Sets GlobalId (for IfcRoot-derived entities). OwnerHistory is assigned only
 * when the owner_history option contains a handle; it is not created
 * automatically. Schema-specific defaults are applied for spatial elements,
 * element types, and door/window styles. If the predefined type is not a valid
 * enum value, it is stored as USERDEFINED with the value in ObjectType
 * (or ElementType/ProcessType where applicable).
 */
IFCAPI_BINDING express::Base root_create_entity(
    ifcopenshell::file* file,
    const RootCreateEntityOptions& options);

/**
 * Remove a product and all its relationships.
 *
 * Performs a deep removal that cleans up: representations, object placements,
 * opening elements, property sets, material assignments, type definitions,
 * space boundaries, nesting relationships, aggregate relationships, spatial
 * containment, element connections, port connections, group memberships,
 * and grid axes. The product entity itself is deleted last.
 */
IFCAPI_BINDING void root_remove_product(
    ifcopenshell::file* file,
    express::Base* product,
    const RootRemoveProductOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ROOT_H */
