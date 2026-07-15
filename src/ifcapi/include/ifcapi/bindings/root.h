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
 * Options for changing an IFC product between occurrence or type classes.
 */
struct RootReassignClassOptions {
    /// Product or type product to replace. Its previous handle becomes invalid.
    express::Base product;
    /// Target IFC class. Defaults to IfcBuildingElementProxy when omitted.
    std::optional<std::string> ifc_class;
    /// Optional predefined type. Unsupported values are stored as USERDEFINED labels.
    std::optional<std::string> predefined_type;
    /// Optional occurrence class used when reassigning a type and its occurrences.
    std::optional<std::string> occurrence_class;
};

/**
 * Create an IFC entity with generated identity, optional name, and optional
 * predefined type.
 *
 * Sets GlobalId (for IfcRoot-derived entities). OwnerHistory is assigned only
 * when the owner_history option is provided; it is not created
 * automatically. Schema-specific defaults are applied for spatial elements,
 * element types, and door/window styles. If the predefined type is not a valid
 * enum value, it is stored as USERDEFINED with the value in ObjectType
 * (or ElementType/ProcessType where applicable).
 */
IFCAPI_BINDING express::Base root_create_entity(
    ifcopenshell::file* file,
    const RootCreateEntityOptions& options);

/**
 * Copy a product with a fresh GlobalId and independent authoring data.
 *
 * Property sets, quantities, placements, nested ports, unfilled openings,
 * material usages, and material sets are copied according to their ownership
 * semantics. Ordinary product representations and type representation maps
 * are omitted. Shared aggregate, containment, type, group, and other
 * applicable inverse relationships retain the copy without duplicating
 * relationship members.
 */
IFCAPI_BINDING express::Base root_copy_class(
    ifcopenshell::file* file,
    express::Base* product);

/**
 * Change a product's class while preserving compatible data and relationships.
 *
 * Related types, sibling occurrences, property sets, representations, and
 * placements are migrated when switching between occurrence and type classes.
 * The replaced entity keeps its STEP id and the old entity handle becomes
 * invalid. Invalid classes or incompatible occurrence/type mappings fail.
 */
IFCAPI_BINDING express::Base root_reassign_class(
    ifcopenshell::file* file,
    const RootReassignClassOptions& options);

/**
 * Remove a product and all its relationships.
 *
 * Removes the product and cleans up its related representations, object placements,
 * opening elements, property sets, material assignments, type definitions,
 * space boundaries, nesting relationships, aggregate relationships, spatial
 * containment, element connections, port connections, group memberships,
 * and grid axes. Related entities are removed only when they are no longer
 * needed by the remaining model.
 */
IFCAPI_BINDING void root_remove_product(
    ifcopenshell::file* file,
    express::Base* product,
    const RootRemoveProductOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ROOT_H */
