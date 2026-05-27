/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SYSTEM_H
#define IFCAPI_BINDINGS_SYSTEM_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base system_add_port(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* element,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base system_add_system(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    IFCAPI_NULLABLE express::Base* owner_history);
IFCAPI_BINDING express::Base system_assign_flow_control(
    ifcopenshell::file* file,
    express::Base* relating_flow_element,
    express::Base* related_flow_control,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base system_assign_port(
    ifcopenshell::file* file,
    express::Base* element,
    express::Base* port,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base system_assign_system(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* system,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void system_connect_port(
    ifcopenshell::file* file,
    express::Base* port1,
    express::Base* port2,
    const std::string& direction,
    IFCAPI_NULLABLE express::Base* element,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void system_disconnect_port(ifcopenshell::file* file, express::Base* port);
IFCAPI_BINDING void system_remove_system(ifcopenshell::file* file, express::Base* system);
IFCAPI_BINDING void system_unassign_flow_control(
    ifcopenshell::file* file,
    express::Base* relating_flow_element,
    express::Base* related_flow_control,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void system_unassign_port(
    ifcopenshell::file* file,
    express::Base* element,
    express::Base* port,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void system_unassign_system(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* system,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SYSTEM_H */
