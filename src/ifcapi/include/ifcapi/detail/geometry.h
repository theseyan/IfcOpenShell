// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_GEOMETRY_H
#define IFCAPI_DETAIL_GEOMETRY_H

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/vector.h"

#include "ifcparse/file.h"

#include <cmath>
#include <cstdint>
#include <vector>
#include <stdexcept>

namespace ifcapi {
namespace detail {

inline std::vector<std::vector<double>> convert_si_to_project_units(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points)
{
    double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    std::vector<std::vector<double>> converted = points;
    for (auto& point : converted) {
        for (double& coordinate : point) {
            coordinate /= unit_scale;
        }
    }
    return converted;
}

inline express::Base create_cartesian_point(
    ifcopenshell::file* file,
    const std::vector<double>& coordinates)
{
    auto point = file->create(file->schema()->declaration_by_name("IfcCartesianPoint"));
    write_double_aggregate(point, "Coordinates", coordinates);
    return point;
}

inline express::Base create_direction(
    ifcopenshell::file* file,
    const std::vector<double>& ratios)
{
    auto direction = file->create(file->schema()->declaration_by_name("IfcDirection"));
    write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

inline std::vector<double> scale_point_coordinates(const std::vector<double>& coordinates, double unit_scale) {
    if (unit_scale == 0.0) {
        throw std::runtime_error("unit_scale must not be zero");
    }
    std::vector<double> result = coordinates;
    for (double& coordinate : result) {
        coordinate /= unit_scale;
    }
    return result;
}

inline express::Base create_scaled_cartesian_point(
    ifcopenshell::file* file,
    const std::vector<double>& coordinates,
    double unit_scale)
{
    return create_cartesian_point(file, scale_point_coordinates(coordinates, unit_scale));
}

inline express::Base create_axis2_placement_3d(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    auto placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    write_ref_attr(placement, "Location", create_cartesian_point(file, location));
    write_ref_attr(placement, "Axis", create_direction(file, axis));
    write_ref_attr(placement, "RefDirection", create_direction(file, ref_direction));
    return placement;
}

inline express::Base create_plane(
    ifcopenshell::file* file,
    express::Base position)
{
    auto plane = file->create(file->schema()->declaration_by_name("IfcPlane"));
    write_ref_attr(plane, "Position", position);
    return plane;
}

inline express::Base create_closed_polyline(
    ifcopenshell::file* file,
    std::vector<std::vector<double>> points,
    double unit_scale)
{
    if (points.empty()) {
        throw std::runtime_error("polyline must contain at least one point");
    }
    if (np_allclose(points.front(), points.back())) {
        points.pop_back();
    }
    if (points.empty()) {
        throw std::runtime_error("polyline must contain at least one distinct point");
    }
    std::vector<express::Base> cartesian_points;
    cartesian_points.reserve(points.size() + 1);
    for (const auto& point : points) {
        cartesian_points.push_back(create_scaled_cartesian_point(file, point, unit_scale));
    }
    cartesian_points.push_back(cartesian_points.front());
    auto polyline = file->create(file->schema()->declaration_by_name("IfcPolyline"));
    write_ref_aggregate(polyline, "Points", cartesian_points);
    return polyline;
}

inline express::Base create_cartesian_point_list(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    size_t dimensions)
{
    const char* ifc_class = dimensions == 2 ? "IfcCartesianPointList2D" : "IfcCartesianPointList3D";
    auto point_list = file->create(file->schema()->declaration_by_name(ifc_class));
    write_double_aggregate_aggregate(point_list, "CoordList", points);
    return point_list;
}

inline express::Base create_polyline_or_indexed_polycurve(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    size_t dimensions,
    bool set_self_intersect = true)
{
    if (file->schema()->name() == "IFC2X3") {
        std::vector<express::Base> cartesian_points;
        for (const auto& point : points) {
            cartesian_points.push_back(create_cartesian_point(file, point));
        }
        auto polyline = file->create(file->schema()->declaration_by_name("IfcPolyline"));
        write_ref_aggregate(polyline, "Points", cartesian_points);
        return polyline;
    }

    auto polycurve = file->create(file->schema()->declaration_by_name("IfcIndexedPolyCurve"));
    write_ref_attr(polycurve, "Points", create_cartesian_point_list(file, points, dimensions));
    if (set_self_intersect) {
        int idx = attr_index_of(polycurve, "SelfIntersect");
        if (idx >= 0) {
            polycurve.set_attribute_value(static_cast<size_t>(idx), false);
        }
    }
    return polycurve;
}

inline bool clipping_allclose3(
    const std::vector<double>& value,
    const std::vector<double>& expected,
    double atol = 1e-2)
{
    if (value.size() != 3 || expected.size() != 3) return false;
    for (size_t i = 0; i < 3; ++i) {
        if (std::fabs(value[i] - expected[i]) > atol) return false;
    }
    return true;
}

inline std::vector<double> clipping_cross3(
    const std::vector<double>& a,
    const std::vector<double>& b)
{
    return {
        a.at(1) * b.at(2) - a.at(2) * b.at(1),
        a.at(2) * b.at(0) - a.at(0) * b.at(2),
        a.at(0) * b.at(1) - a.at(1) * b.at(0),
    };
}

inline void clipping_normalize(std::vector<double>& value) {
    const double length = std::sqrt(value.at(0) * value.at(0) + value.at(1) * value.at(1) + value.at(2) * value.at(2));
    for (double& component : value) {
        component /= length;
    }
}

inline std::vector<double> clipping_x_axis(const std::vector<double>& normal) {
    const std::vector<double> arbitrary =
        (clipping_allclose3(normal, {0.0, 0.0, 1.0}) || clipping_allclose3(normal, {0.0, 0.0, -1.0}))
            ? std::vector<double>{0.0, 1.0, 0.0}
            : std::vector<double>{0.0, 0.0, 1.0};
    auto result = clipping_cross3(normal, arbitrary);
    clipping_normalize(result);
    return result;
}

inline express::Base create_clipping_plane(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    double unit_scale)
{
    auto scaled_location = location;
    for (auto& coordinate : scaled_location) coordinate /= unit_scale;
    auto plane = file->create(file->schema()->declaration_by_name("IfcPlane"));
    write_ref_attr(
        plane,
        "Position",
        create_axis2_placement_3d(file, scaled_location, normal, clipping_x_axis(normal)));
    return plane;
}

inline express::Base create_clipping_result(
    ifcopenshell::file* file,
    express::Base first_operand,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    double unit_scale)
{
    auto half_space = file->create(file->schema()->declaration_by_name("IfcHalfSpaceSolid"));
    write_ref_attr(half_space, "BaseSurface", create_clipping_plane(file, location, normal, unit_scale));
    entity_view(half_space).set("AgreementFlag", false);

    auto result = file->create(file->schema()->declaration_by_name("IfcBooleanClippingResult"));
    write_string_attr(result, "Operator", "DIFFERENCE");
    write_ref_attr(result, "FirstOperand", first_operand);
    write_ref_attr(result, "SecondOperand", half_space);
    return result;
}

inline express::Base copy_boolean_clipping(
    ifcopenshell::file* file,
    express::Base clipping,
    express::Base first_operand)
{
    auto copy = copy_single(file, clipping);
    if (!copy) throw std::runtime_error("Unable to copy clipping entity");
    write_ref_attr(copy, "FirstOperand", first_operand);
    return copy;
}

/** Apply semantic clippings in reverse input order, copying entity clippings. */
inline express::Base apply_ordered_clippings(
    ifcopenshell::file* file,
    express::Base first_operand,
    const std::vector<bindings::GeometryClipping>& clippings,
    double unit_scale)
{
    for (auto it = clippings.rbegin(); it != clippings.rend(); ++it) {
        if (const auto* plane = std::get_if<bindings::GeometryPlaneClipping>(&*it)) {
            first_operand = create_clipping_result(
                file,
                first_operand,
                std::vector<double>(plane->location.begin(), plane->location.end()),
                std::vector<double>(plane->normal.begin(), plane->normal.end()),
                unit_scale);
        } else {
            first_operand = copy_boolean_clipping(
                file, std::get<bindings::GeometryEntityClipping>(*it).entity, first_operand);
        }
    }
    return first_operand;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_GEOMETRY_H */
