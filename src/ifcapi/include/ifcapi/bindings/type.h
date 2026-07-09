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
    std::vector<express::Base> objects;
    express::Base relating_type;
    std::optional<bool> should_map_representations = true;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning types from element occurrences.
 */
struct TypeUnassignTypeOptions {
    std::vector<express::Base> objects;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Assign a type to element occurrences, creating or merging IfcRelDefinesByType.
 */
IFCAPI_BINDING express::Base type_assign_type(
    ifcopenshell::file* file,
    const TypeAssignTypeOptions& options);

/**
 * Map representation maps from a type onto a related object.
 */
IFCAPI_BINDING bool type_map_type_representations(
    ifcopenshell::file* file,
    express::Base* related_object,
    express::Base* relating_type);

/**
 * Remove type assignments from element occurrences.
 */
IFCAPI_BINDING void type_unassign_type(
    ifcopenshell::file* file,
    const TypeUnassignTypeOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_TYPE_H */
