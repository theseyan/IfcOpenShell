/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GEOMETRY_H
#define IFCAPI_BINDINGS_GEOMETRY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_assign_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_map_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING void geometry_unassign_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_copy_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* source,
    IfcUtil::IfcBaseClass* target,
    const char* context_identifier);
IFCAPI_BINDING std::vector<double> geometry_profile_extents(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_edit_object_placement(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    const std::vector<double>& matrix,
    bool is_si,
    bool should_transform_children);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOMETRY_H */
