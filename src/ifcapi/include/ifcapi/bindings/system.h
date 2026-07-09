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
    std::optional<express::Base> element;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for creating a new system.
 */
struct SystemAddSystemOptions {
    std::string ifc_class;
    std::optional<express::Base> owner_history;
};

/**
 * Options for assigning a flow control element to a flow element.
 */
struct SystemAssignFlowControlOptions {
    express::Base relating_flow_element;
    express::Base related_flow_control;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for assigning a port to an element.
 */
struct SystemAssignPortOptions {
    express::Base element;
    express::Base port;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for assigning products to a system.
 */
struct SystemAssignSystemOptions {
    std::vector<express::Base> products;
    express::Base system;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for connecting two ports together.
 */
struct SystemConnectPortOptions {
    express::Base port1;
    express::Base port2;
    std::string direction;
    std::optional<express::Base> element;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a flow control element from a flow element.
 */
struct SystemUnassignFlowControlOptions {
    express::Base relating_flow_element;
    express::Base related_flow_control;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a port from an element.
 */
struct SystemUnassignPortOptions {
    express::Base element;
    express::Base port;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning products from a system.
 */
struct SystemUnassignSystemOptions {
    std::vector<express::Base> products;
    express::Base system;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Create a new distribution port and optionally assign it to an element.
 */
IFCAPI_BINDING express::Base system_add_port(
    ifcopenshell::file* file,
    const SystemAddPortOptions& options);

/**
 * Create a new system with the given IFC class.
 */
IFCAPI_BINDING express::Base system_add_system(
    ifcopenshell::file* file,
    const SystemAddSystemOptions& options);

/**
 * Assign a flow control element to a flow element.
 */
IFCAPI_BINDING express::Base system_assign_flow_control(
    ifcopenshell::file* file,
    const SystemAssignFlowControlOptions& options);

/**
 * Assign a port to an element.
 */
IFCAPI_BINDING express::Base system_assign_port(
    ifcopenshell::file* file,
    const SystemAssignPortOptions& options);

/**
 * Assign products to a system.
 */
IFCAPI_BINDING express::Base system_assign_system(
    ifcopenshell::file* file,
    const SystemAssignSystemOptions& options);

/**
 * Connect two ports together with a given flow direction.
 */
IFCAPI_BINDING void system_connect_port(
    ifcopenshell::file* file,
    const SystemConnectPortOptions& options);

/**
 * Disconnect a port from all connected ports.
 */
IFCAPI_BINDING void system_disconnect_port(ifcopenshell::file* file, express::Base* port);

/**
 * Remove a system and its relationships.
 */
IFCAPI_BINDING void system_remove_system(ifcopenshell::file* file, express::Base* system);

/**
 * Unassign a flow control element from a flow element.
 */
IFCAPI_BINDING void system_unassign_flow_control(
    ifcopenshell::file* file,
    const SystemUnassignFlowControlOptions& options);

/**
 * Unassign a port from an element.
 */
IFCAPI_BINDING void system_unassign_port(
    ifcopenshell::file* file,
    const SystemUnassignPortOptions& options);

/**
 * Unassign products from a system.
 */
IFCAPI_BINDING void system_unassign_system(
    ifcopenshell::file* file,
    const SystemUnassignSystemOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SYSTEM_H */
