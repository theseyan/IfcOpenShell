/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTROL_H
#define IFCAPI_BINDINGS_CONTROL_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning a control to related objects.
 */
struct ControlAssignControlOptions {
    express::Base relating_control;
    std::vector<express::Base> related_objects;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for removing a control from related objects.
 */
struct ControlUnassignControlOptions {
    express::Base relating_control;
    std::vector<express::Base> related_objects;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Assign a planning control or constraint to a list of objects.
 */
IFCAPI_BINDING express::Base control_assign_control(
    ifcopenshell::file* file,
    const ControlAssignControlOptions& options);

/**
 * Remove a planning control or constraint from a list of objects.
 */
IFCAPI_BINDING void control_unassign_control(
    ifcopenshell::file* file,
    const ControlUnassignControlOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTROL_H */
