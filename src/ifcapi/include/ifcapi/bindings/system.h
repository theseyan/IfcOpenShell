/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SYSTEM_H
#define IFCAPI_BINDINGS_SYSTEM_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a distribution port to an element.
 */
struct SystemAddPortOptions {
    /// Optional element to assign the port to. If omitted, the port is created
    /// standalone and must be assigned later via system_assign_port.
    std::optional<express::Base> element;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for creating a new system.
 */
struct SystemAddSystemOptions {
    /// IFC class for the system (e.g. "IfcDistributionSystem", "IfcZone",
    /// "IfcBuildingSystem"). In IFC2X3, "IfcDistributionSystem" is
    /// automatically mapped to "IfcSystem".
    std::string ifc_class;
    /// Optional owner history.
    std::optional<express::Base> owner_history;
};

/**
 * Options for assigning a flow control element to a flow element.
 */
struct SystemAssignFlowControlOptions {
    /// The flow element (e.g. IfcFlowSegment) that owns the control.
    express::Base relating_flow_element;
    /// The flow control element (e.g. IfcFlowController) to assign.
    express::Base related_flow_control;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for assigning a port to an element.
 */
struct SystemAssignPortOptions {
    /// The element to assign the port to.
    express::Base element;
    /// The IfcDistributionPort to assign.
    express::Base port;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for assigning products to a system.
 */
struct SystemAssignSystemOptions {
    /// Products to assign to the system. Must be compatible with the system type.
    std::vector<express::Base> products;
    /// The system (IfcSystem subclass or IfcGroup) to assign products to.
    express::Base system;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for connecting two ports together.
 */
struct SystemConnectPortOptions {
    /// The first port in the connection.
    express::Base port1;
    /// The second port in the connection.
    express::Base port2;
    /// Flow direction: "SOURCE" (port1 sources, port2 sinks), "SINK" (port1
    /// sinks, port2 sources), "SOURCEANDSINK", or "NOTDEFINED".
    std::string direction;
    /// Optional realizing element (e.g. a pipe or duct connecting the ports).
    std::optional<express::Base> element;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a flow control element from a flow element.
 */
struct SystemUnassignFlowControlOptions {
    /// The flow element that owns the control.
    express::Base relating_flow_element;
    /// The flow control element to unassign.
    express::Base related_flow_control;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a port from an element.
 */
struct SystemUnassignPortOptions {
    /// The element the port is assigned to.
    express::Base element;
    /// The IfcDistributionPort to unassign.
    express::Base port;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning products from a system.
 */
struct SystemUnassignSystemOptions {
    /// Products to remove from the system.
    std::vector<express::Base> products;
    /// The system to remove products from.
    express::Base system;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Create a new IfcDistributionPort and optionally assign it to an element.
 *
 * If an element is provided, the port is assigned to it via IfcRelNests
 * (IFC4+) or IfcRelConnectsPortToElement (IFC2X3).
 */
IFCAPI_BINDING express::Base system_add_port(
    ifcopenshell::file* file,
    const SystemAddPortOptions& options);

/**
 * Create a new system with the given IFC class.
 *
 * In IFC2X3, "IfcDistributionSystem" is automatically resolved to "IfcSystem".
 * The system is created with Name set to "Unnamed".
 */
IFCAPI_BINDING express::Base system_add_system(
    ifcopenshell::file* file,
    const SystemAddSystemOptions& options);

/**
 * Assign a flow control element to a flow element via IfcRelFlowControlElements.
 *
 * If the flow control is already assigned to a different element, no change
 * is made and no relationship is returned.
 */
IFCAPI_BINDING express::Base system_assign_flow_control(
    ifcopenshell::file* file,
    const SystemAssignFlowControlOptions& options);

/**
 * Assign a port to an element.
 *
 * In IFC2X3, uses IfcRelConnectsPortToElement. In IFC4+, uses IfcRelNests
 * to nest the port under the element. Updates the port's ObjectPlacement
 * if it has an IfcLocalPlacement.
 */
IFCAPI_BINDING express::Base system_assign_port(
    ifcopenshell::file* file,
    const SystemAssignPortOptions& options);

/**
 * Assign products to a system via IfcRelAssignsToGroup.
 *
 * Validates that each product is compatible with the system type (e.g.
 * only IfcDistributionElement for IfcDistributionSystem). The operation fails
 * if a product is not valid for the system type.
 */
IFCAPI_BINDING express::Base system_assign_system(
    ifcopenshell::file* file,
    const SystemAssignSystemOptions& options);

/**
 * Connect two ports together with a given flow direction.
 *
 * Creates IfcRelConnectsPorts relationships and sets FlowDirection on both
 * ports. Existing connections to other ports are removed first. The
 * direction parameter controls which port is SOURCE, SINK, or both.
 */
IFCAPI_BINDING void system_connect_port(
    ifcopenshell::file* file,
    const SystemConnectPortOptions& options);

/**
 * Disconnect a port from all connected ports.
 *
 * Removes all IfcRelConnectsPorts relationships involving the port and
 * clears FlowDirection on both sides.
 */
IFCAPI_BINDING void system_disconnect_port(ifcopenshell::file* file, express::Base* port);

/**
 * Remove a system and its relationships.
 *
 * Removes associated IfcRelDefinesByProperties (property sets) and
 * IfcRelAssignsToGroup relationships, then deletes the system entity.
 */
IFCAPI_BINDING void system_remove_system(ifcopenshell::file* file, express::Base* system);

/**
 * Remove a flow control element from a flow element's IfcRelFlowControlElements.
 *
 * If the control element is the only one in the relationship, the
 * relationship is deleted. Otherwise, the control element is removed from
 * the RelatedControlElements aggregate.
 */
IFCAPI_BINDING void system_unassign_flow_control(
    ifcopenshell::file* file,
    const SystemUnassignFlowControlOptions& options);

/**
 * Remove a port from an element.
 *
 * In IFC2X3, removes the IfcRelConnectsPortToElement. In IFC4+, removes
 * the port from the IfcRelNests relationship (or deletes the relationship
 * if it was the only nested object).
 */
IFCAPI_BINDING void system_unassign_port(
    ifcopenshell::file* file,
    const SystemUnassignPortOptions& options);

/**
 * Remove products from a system.
 */
IFCAPI_BINDING void system_unassign_system(
    ifcopenshell::file* file,
    const SystemUnassignSystemOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SYSTEM_H */
