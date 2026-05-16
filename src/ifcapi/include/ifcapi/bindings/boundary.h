/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_BOUNDARY_H
#define IFCAPI_BINDINGS_BOUNDARY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* boundary_copy_boundary(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* boundary);
IFCAPI_BINDING void boundary_remove_boundary(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* boundary);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_BOUNDARY_H */
