/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GROUP_H
#define IFCAPI_BINDINGS_GROUP_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* group_add_group(
    IfcParse::IfcFile* file,
    const std::string& name,
    const char* description,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* group_update_group_products(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* group,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* group_assign_group(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* group,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void group_unassign_group(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* group,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void group_remove_group(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* group);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GROUP_H */
