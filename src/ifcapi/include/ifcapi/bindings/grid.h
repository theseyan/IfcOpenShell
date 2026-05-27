/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GRID_H
#define IFCAPI_BINDINGS_GRID_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base grid_create_grid_axis(
    ifcopenshell::file* file,
    express::Base* grid,
    const std::string& axis_tag,
    bool same_sense,
    const std::string& uvw_axes);
IFCAPI_BINDING void grid_remove_grid_axis(
    ifcopenshell::file* file,
    express::Base* axis);
IFCAPI_BINDING void grid_create_axis_curve(
    ifcopenshell::file* file,
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    express::Base* grid_axis,
    bool is_si);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GRID_H */
