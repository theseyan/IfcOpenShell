// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/grid.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/geometry.h"
#include "placement_helpers.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/schema.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

express::Base owning_grid(ifcopenshell::file* file, express::Base grid_axis) {
    if (!file || !grid_axis || grid_axis.id() == 0) {
        return {};
    }
    for (auto inverse : file->instances_by_reference(static_cast<int>(grid_axis.id()))) {
        if (inverse && inverse.declaration().is("IfcGrid")) {
            return inverse;
        }
    }
    return {};
}

std::vector<double> normalise_point(const std::vector<double>& point) {
    if (point.size() < 3) {
        throw std::runtime_error("Grid axis curve points must have at least three coordinates");
    }
    return {point[0], point[1], point[2]};
}

std::vector<double> apply_matrix(const double* matrix, const std::vector<double>& point) {
    return {
        matrix[0] * point[0] + matrix[1] * point[1] + matrix[2] * point[2] + matrix[3],
        matrix[4] * point[0] + matrix[5] * point[1] + matrix[6] * point[2] + matrix[7],
        matrix[8] * point[0] + matrix[9] * point[1] + matrix[10] * point[2] + matrix[11],
    };
}

express::Base create_axis_polyline(
    ifcopenshell::file* file,
    const std::vector<double>& p1,
    const std::vector<double>& p2)
{
    auto polyline = file->create(file->schema()->declaration_by_name("IfcPolyline"));
    ifcapi::detail::write_ref_aggregate(polyline, "Points", {
        ifcapi::detail::create_cartesian_point(file, {p1[0], p1[1]}),
        ifcapi::detail::create_cartesian_point(file, {p2[0], p2[1]}),
    });
    return polyline;
}
}

namespace ifcapi {
namespace bindings {

express::Base grid_create_grid_axis(
    ifcopenshell::file* file,
    express::Base* grid,
    const std::string& axis_tag,
    bool same_sense,
    const std::string& uvw_axes)
{
    ifcopenshell_clear_error();
    try {
        auto grid_value = ifcapi::detail::deref_or_empty(grid);
        if (!file || !grid_value) return {};
        int axes_idx = ifcapi::detail::attr_index_of(grid_value, uvw_axes.c_str());
        if (axes_idx < 0) {
            set_error("Invalid grid axis aggregate");
            return {};
        }
        auto axis = file->create(file->schema()->declaration_by_name("IfcGridAxis"));
        ifcapi::detail::write_string_attr(axis, "AxisTag", axis_tag);
        if (int same_sense_idx = ifcapi::detail::attr_index_of(axis, "SameSense"); same_sense_idx >= 0) {
            axis.set_attribute_value(static_cast<size_t>(same_sense_idx), same_sense);
        }
        auto axes = ifcapi::detail::read_ref_aggregate(grid_value, uvw_axes.c_str());
        axes.push_back(axis);
        ifcapi::detail::set_ref_aggregate(grid_value, axes_idx, axes);
        return axis;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

void grid_remove_grid_axis(ifcopenshell::file* file, express::Base* axis) {
    ifcopenshell_clear_error();
    try {
        auto axis_value = ifcapi::detail::deref_or_empty(axis);
        auto axis_curve = ifcapi::detail::read_ref_attr(axis_value, "AxisCurve");
        file->remove_entity(axis_value);
        if (axis_curve) entity_remove_deep(&axis_curve);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void grid_create_axis_curve(
    ifcopenshell::file* file,
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    express::Base* grid_axis,
    bool is_si)
{
    ifcopenshell_clear_error();
    try {
        auto grid_axis_value = ifcapi::detail::deref_or_empty(grid_axis);
        if (!file || !grid_axis_value) {
            throw std::runtime_error("grid_create_axis_curve requires a file and grid axis");
        }
        auto point1 = normalise_point(p1);
        auto point2 = normalise_point(p2);
        if (is_si) {
            double unit_scale = unit_calculate_unit_scale(file, "LENGTHUNIT");
            for (double* value : {&point1[0], &point1[1], &point1[2], &point2[0], &point2[1], &point2[2]}) {
                *value /= unit_scale;
            }
        }
        auto grid = owning_grid(file, grid_axis_value);
        if (!grid) {
            throw std::runtime_error("Unable to find owning IfcGrid for grid axis");
        }
        double matrix[16];
        ifcapi::identity4(matrix);
        if (auto placement = ifcapi::detail::read_ref_attr(grid, "ObjectPlacement")) {
            if (!ifcapi::compute_local_placement(placement, matrix)) {
                throw std::runtime_error("Unable to compute grid placement matrix");
            }
        }
        double inverse[16];
        if (!ifcapi::invert4(matrix, inverse)) {
            throw std::runtime_error("Unable to invert grid placement matrix");
        }
        point1 = apply_matrix(inverse, point1);
        point2 = apply_matrix(inverse, point2);

        auto existing_curve = ifcapi::detail::read_ref_attr(grid_axis_value, "AxisCurve");
        ifcapi::detail::write_ref_attr(grid_axis_value, "AxisCurve", create_axis_polyline(file, point1, point2));
        if (existing_curve) {
            entity_remove_deep(&existing_curve);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

} // namespace bindings
} // namespace ifcapi
