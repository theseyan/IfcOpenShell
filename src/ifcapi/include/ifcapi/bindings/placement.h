/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PLACEMENT_H
#define IFCAPI_BINDINGS_PLACEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::vector<double> placement_matrix_from_axes(
    const std::vector<double>& origin,
    const std::vector<double>& z_axis,
    const std::vector<double>& x_axis);
IFCAPI_BINDING std::vector<double> placement_get_axis2_placement(express::Base* instance);
IFCAPI_BINDING std::vector<double> placement_get_local_placement(
    std::optional<express::Base> instance);
IFCAPI_BINDING std::vector<double> placement_get_cartesian_xform_3d(express::Base* instance);
IFCAPI_BINDING std::vector<double> placement_get_mappeditem_xform(express::Base* instance);
IFCAPI_BINDING double placement_get_storey_elevation(express::Base* instance);
IFCAPI_BINDING std::vector<double> placement_rotation(double angle_rad, const std::string& axis);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PLACEMENT_H */
