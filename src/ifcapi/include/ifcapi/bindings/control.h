/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTROL_H
#define IFCAPI_BINDINGS_CONTROL_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* control_assign_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_control,
    const std::vector<const IfcUtil::IfcBaseClass*>& related_objects,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void control_unassign_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_control,
    const std::vector<const IfcUtil::IfcBaseClass*>& related_objects,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTROL_H */
