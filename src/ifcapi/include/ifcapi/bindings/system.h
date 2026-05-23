/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SYSTEM_H
#define IFCAPI_BINDINGS_SYSTEM_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* system_add_port(
    IfcParse::IfcFile* file,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* element,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* system_add_system(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* system_assign_flow_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_flow_element,
    IfcUtil::IfcBaseClass* related_flow_control,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* system_assign_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* port,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* system_assign_system(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* system,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void system_connect_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* port1,
    IfcUtil::IfcBaseClass* port2,
    const std::string& direction,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* element,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void system_disconnect_port(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* port);
IFCAPI_BINDING void system_remove_system(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* system);
IFCAPI_BINDING void system_unassign_flow_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_flow_element,
    IfcUtil::IfcBaseClass* related_flow_control,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void system_unassign_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* port,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void system_unassign_system(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* system,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SYSTEM_H */
