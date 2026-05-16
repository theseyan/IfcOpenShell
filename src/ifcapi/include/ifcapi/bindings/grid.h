/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GRID_H
#define IFCAPI_BINDINGS_GRID_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* grid_create_grid_axis(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* grid,
    const std::string& axis_tag,
    bool same_sense,
    const std::string& uvw_axes);
IFCAPI_BINDING void grid_remove_grid_axis(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* axis);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GRID_H */
