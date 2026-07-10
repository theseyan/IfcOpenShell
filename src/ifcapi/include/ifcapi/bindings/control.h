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
    /// The planning control (e.g. IfcTask, IfcCostItem) to assign.
    express::Base relating_control;
    /// Objects to associate with the control.
    std::vector<express::Base> related_objects;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing a control from related objects.
 */
struct ControlUnassignControlOptions {
    /// The planning control to unassign from.
    express::Base relating_control;
    /// Objects to disassociate from the control.
    std::vector<express::Base> related_objects;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Assign a planning control or constraint to objects via IfcRelAssignsToControl.
 *
 * Objects already assigned to the control are skipped. If an existing
 * IfcRelAssignsToControl relationship exists for the control, new objects
 * are merged into it.
 */
IFCAPI_BINDING express::Base control_assign_control(
    ifcopenshell::file* file,
    const ControlAssignControlOptions& options);

/**
 * Remove objects from an IfcRelAssignsToControl relationship.
 *
 * If no related objects remain after removal, the relationship is deleted.
 */
IFCAPI_BINDING void control_unassign_control(
    ifcopenshell::file* file,
    const ControlUnassignControlOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTROL_H */
