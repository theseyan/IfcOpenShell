// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_GEOMETRY_H
#define IFCAPI_DETAIL_GEOMETRY_H

#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/vector.h"

#include "ifcparse/IfcFile.h"

#include <vector>
#include <stdexcept>

namespace ifcapi {
namespace detail {

inline std::vector<std::vector<double>> convert_si_to_project_units(
    IfcParse::IfcFile* file,
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

inline IfcUtil::IfcBaseClass* create_cartesian_point(
    IfcParse::IfcFile* file,
    const std::vector<double>& coordinates)
{
    auto* point = file->create(file->schema()->declaration_by_name("IfcCartesianPoint"));
    write_double_aggregate(point, "Coordinates", coordinates);
    return point;
}

inline IfcUtil::IfcBaseClass* create_direction(
    IfcParse::IfcFile* file,
    const std::vector<double>& ratios)
{
    auto* direction = file->create(file->schema()->declaration_by_name("IfcDirection"));
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

inline IfcUtil::IfcBaseClass* create_scaled_cartesian_point(
    IfcParse::IfcFile* file,
    const std::vector<double>& coordinates,
    double unit_scale)
{
    return create_cartesian_point(file, scale_point_coordinates(coordinates, unit_scale));
}

inline IfcUtil::IfcBaseClass* create_axis2_placement_3d(
    IfcParse::IfcFile* file,
    const std::vector<double>& location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    auto* placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    write_ref_attr(placement, "Location", create_cartesian_point(file, location));
    write_ref_attr(placement, "Axis", create_direction(file, axis));
    write_ref_attr(placement, "RefDirection", create_direction(file, ref_direction));
    return placement;
}

inline IfcUtil::IfcBaseClass* create_plane(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* position)
{
    auto* plane = file->create(file->schema()->declaration_by_name("IfcPlane"));
    write_ref_attr(plane, "Position", position);
    return plane;
}

inline IfcUtil::IfcBaseClass* create_closed_polyline(
    IfcParse::IfcFile* file,
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
    std::vector<IfcUtil::IfcBaseClass*> cartesian_points;
    cartesian_points.reserve(points.size() + 1);
    for (const auto& point : points) {
        cartesian_points.push_back(create_scaled_cartesian_point(file, point, unit_scale));
    }
    cartesian_points.push_back(cartesian_points.front());
    auto* polyline = file->create(file->schema()->declaration_by_name("IfcPolyline"));
    write_ref_aggregate(polyline, "Points", cartesian_points);
    return polyline;
}

inline IfcUtil::IfcBaseClass* create_cartesian_point_list(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& points,
    size_t dimensions)
{
    const char* ifc_class = dimensions == 2 ? "IfcCartesianPointList2D" : "IfcCartesianPointList3D";
    auto* point_list = file->create(file->schema()->declaration_by_name(ifc_class));
    write_double_aggregate_aggregate(point_list, "CoordList", points);
    return point_list;
}

inline IfcUtil::IfcBaseClass* create_polyline_or_indexed_polycurve(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& points,
    size_t dimensions,
    bool set_self_intersect = true)
{
    if (file->schema()->name() == "IFC2X3") {
        std::vector<IfcUtil::IfcBaseClass*> cartesian_points;
        for (const auto& point : points) {
            cartesian_points.push_back(create_cartesian_point(file, point));
        }
        auto* polyline = file->create(file->schema()->declaration_by_name("IfcPolyline"));
        write_ref_aggregate(polyline, "Points", cartesian_points);
        return polyline;
    }

    auto* polycurve = file->create(file->schema()->declaration_by_name("IfcIndexedPolyCurve"));
    write_ref_attr(polycurve, "Points", create_cartesian_point_list(file, points, dimensions));
    if (set_self_intersect) {
        int idx = attr_index_of(polycurve, "SelfIntersect");
        if (idx >= 0) {
            polycurve->set_attribute_value(static_cast<size_t>(idx), false);
        }
    }
    return polycurve;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_GEOMETRY_H */
