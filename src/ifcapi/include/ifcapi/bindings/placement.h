/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PLACEMENT_H
#define IFCAPI_BINDINGS_PLACEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::vector<double> placement_a2p(
    const std::vector<double>& origin,
    const std::vector<double>& z_axis,
    const std::vector<double>& x_axis);
IFCAPI_BINDING std::vector<double> placement_get_axis2placement(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING std::vector<double> placement_get_local_placement(
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING std::vector<double> placement_get_cartesian_xform_3d(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING std::vector<double> placement_get_mappeditem_xform(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING double placement_get_storey_elevation(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING std::vector<double> placement_rotation(double angle_rad, const std::string& axis);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PLACEMENT_H */
