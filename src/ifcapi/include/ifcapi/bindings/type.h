/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_TYPE_H
#define IFCAPI_BINDINGS_TYPE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* type_assign_type(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& objects,
    IfcUtil::IfcBaseClass* relating_type,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* type_assign_type_ex(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& objects,
    IfcUtil::IfcBaseClass* relating_type,
    bool should_map_representations,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING bool type_map_type_representations(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* relating_type);
IFCAPI_BINDING void type_unassign_type(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& objects,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_TYPE_H */
