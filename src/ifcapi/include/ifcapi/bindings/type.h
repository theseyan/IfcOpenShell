/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_TYPE_H
#define IFCAPI_BINDINGS_TYPE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning a type to element occurrences.
 */
struct TypeAssignTypeOptions {
    /// Element occurrences to assign the type to.
    std::vector<express::Base> objects;
    /// The IfcTypeObject to assign.
    express::Base relating_type;
    /// If true (default), IfcRepresentationMaps from the type are propagated
    /// onto each occurrence, and PredefinedType/ObjectType are cleared on
    /// objects when the type already declares a non-NOTDEFINED PredefinedType.
    std::optional<bool> should_map_representations = true;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning types from element occurrences.
 */
struct TypeUnassignTypeOptions {
    /// Element occurrences to remove type assignments from.
    std::vector<express::Base> objects;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Assign a type to element occurrences via IfcRelDefinesByType.
 *
 * Objects previously typed by a different type are moved. If an existing
 * IfcRelDefinesByType relationship already relates to the same type,
 * objects are merged into it. When should_map_representations is true
 * (default), IfcRepresentationMaps from the type are propagated onto
 * each occurrence and PredefinedType/ObjectType are cleared to avoid
 * double-typing.
 */
IFCAPI_BINDING express::Base type_assign_type(
    ifcopenshell::file* file,
    const TypeAssignTypeOptions& options);

/**
 * Map IfcRepresentationMaps from a type onto a related object.
 *
 * Clones mapped representations from the type's RepresentationMaps and
 * assigns them to the object. Returns true if any representations were
 * mapped.
 */
IFCAPI_BINDING bool type_map_type_representations(
    ifcopenshell::file* file,
    express::Base* related_object,
    express::Base* relating_type);

/**
 * Remove type assignments from element occurrences.
 *
 * Removes objects from IfcRelDefinesByType relationships. If no objects
 * remain in a relationship, the relationship is deleted.
 */
IFCAPI_BINDING void type_unassign_type(
    ifcopenshell::file* file,
    const TypeUnassignTypeOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_TYPE_H */
