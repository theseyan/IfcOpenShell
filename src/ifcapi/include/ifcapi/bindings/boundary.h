/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_BOUNDARY_H
#define IFCAPI_BINDINGS_BOUNDARY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base boundary_copy_boundary(
    ifcopenshell::file* file,
    express::Base* boundary);
IFCAPI_BINDING void boundary_assign_connection_geometry(
    ifcopenshell::file* file,
    express::Base* rel_space_boundary,
    const std::vector<std::vector<double>>& outer_boundary,
    const std::vector<double>& location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction,
    const std::vector<std::vector<std::vector<double>>>& inner_boundaries,
    double unit_scale);
IFCAPI_BINDING void boundary_remove_boundary(
    ifcopenshell::file* file,
    express::Base* boundary);
IFCAPI_BINDING void boundary_edit_attributes(
    express::Base* entity,
    express::Base* relating_space,
    express::Base* related_building_element,
    IFCAPI_NULLABLE express::Base* parent_boundary,
    IFCAPI_NULLABLE express::Base* corresponding_boundary,
    const std::string& physical_or_virtual,
    const std::string& internal_or_external);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_BOUNDARY_H */
