// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_GEOMETRY_H
#define IFCAPI_DETAIL_GEOMETRY_H

#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/IfcFile.h"

#include <vector>

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
