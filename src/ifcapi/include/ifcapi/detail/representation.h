// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_REPRESENTATION_H
#define IFCAPI_DETAIL_REPRESENTATION_H

#include "ifcapi/bindings/representation.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/file.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace ifcapi {
namespace detail {

inline bool is_a(express::Base entity, const char* name) {
    return entity && entity.declaration().is(name);
}

inline bool read_curve_axis_points(express::Base curve, std::vector<double>& p0, std::vector<double>& p1) {
    if (is_a(curve, "IfcPolyline")) {
        auto points = read_ref_aggregate(curve, "Points");
        if (points.size() < 2) {
            return false;
        }
        p0 = read_double_aggregate(points[0], "Coordinates");
        p1 = read_double_aggregate(points[1], "Coordinates");
        return p0.size() >= 2 && p1.size() >= 2;
    }
    if (is_a(curve, "IfcIndexedPolyCurve")) {
        auto points = read_ref_attr(curve, "Points");
        if (!points) {
            return false;
        }
        int idx = attr_index_of(points, "CoordList");
        if (idx < 0) {
            return false;
        }
        try {
            auto value = points.get_attribute_value(static_cast<size_t>(idx));
            if (value.isNull()) {
                return false;
            }
            auto coords = static_cast<std::vector<std::vector<double>>>(value);
            if (coords.size() < 2) {
                return false;
            }
            p0 = coords[0];
            p1 = coords[1];
            return p0.size() >= 2 && p1.size() >= 2;
        } catch (...) {
            return false;
        }
    }
    return false;
}

inline std::vector<std::vector<double>> get_reference_line(ifcopenshell::file* file, express::Base wall) {
    auto wall_ref = wall;
    if (auto axis = ifcapi::bindings::representation_get_product_representation(
            &wall_ref, {{}, "Plan", "Axis", "GRAPH_VIEW"})) {
        if (auto resolved = ifcapi::bindings::representation_resolve(&axis)) {
            for (auto item : read_ref_aggregate(resolved, "Items")) {
                std::vector<double> p0, p1;
                if (!read_curve_axis_points(item, p0, p1)) {
                    continue;
                }
                if (p0[0] < p1[0]) {
                    return {{p0[0], p0[1]}, {p1[0], p1[1]}};
                }
                return {{p1[0], p1[1]}, {p0[0], p0[1]}};
            }
        }
    }

    auto definition = read_ref_attr(wall, "Representation");
    for (auto representation : read_ref_aggregate(definition, "Representations")) {
        for (auto item : read_ref_aggregate(representation, "Items")) {
            std::vector<express::Base> candidates = {item};
            if (file && item) {
                candidates = ifcopenshell::file::traverse(item, -1);
            }
            for (auto candidate : candidates) {
                if (!is_a(candidate, "IfcExtrudedAreaSolid")) {
                    continue;
                }
                auto profile = read_ref_attr(candidate, "SweptArea");
                auto curve = read_ref_attr(profile, "OuterCurve");
                std::vector<double> x_values;
                if (is_a(curve, "IfcPolyline")) {
                    for (auto point : read_ref_aggregate(curve, "Points")) {
                        auto coords = read_double_aggregate(point, "Coordinates");
                        if (!coords.empty()) {
                            x_values.push_back(coords[0]);
                        }
                    }
                } else if (is_a(curve, "IfcIndexedPolyCurve")) {
                    auto points = read_ref_attr(curve, "Points");
                    int idx = attr_index_of(points, "CoordList");
                    if (idx >= 0) {
                        try {
                            auto value = points.get_attribute_value(static_cast<size_t>(idx));
                            if (!value.isNull()) {
                                for (const auto& coords : static_cast<std::vector<std::vector<double>>>(value)) {
                                    if (!coords.empty()) {
                                        x_values.push_back(coords[0]);
                                    }
                                }
                            }
                        } catch (...) {
                        }
                    }
                }
                if (!x_values.empty()) {
                    auto minmax = std::minmax_element(x_values.begin(), x_values.end());
                    return {{*minmax.first, 0.0}, {*minmax.second, 0.0}};
                }
            }
        }
    }
    return {{0.0, 0.0}, {1.0, 0.0}};
}

inline bool intersect_x_axis_2d(
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    double y,
    double& x)
{
    if (std::fabs(p1[1] - p2[1]) <= 1e-5) {
        return false;
    }
    const double t = (y - p1[1]) / (p2[1] - p1[1]);
    x = p1[0] + t * (p2[0] - p1[0]);
    return true;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_REPRESENTATION_H */
