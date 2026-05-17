// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_SHAPE_BUILDER_H
#define IFCAPI_DETAIL_SHAPE_BUILDER_H

#include "ifcapi/bindings/shape_builder.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <stdexcept>
#include <vector>

namespace ifcapi {
namespace detail {

inline std::vector<const IfcUtil::IfcBaseClass*> const_refs(const std::vector<IfcUtil::IfcBaseClass*>& items) {
    return {items.begin(), items.end()};
}

inline std::vector<double> v2(double x, double y) {
    return {x, y};
}

inline std::vector<double> v3(double x, double y, double z) {
    return {x, y, z};
}

inline std::vector<double> add3(const std::vector<double>& a, const std::vector<double>& b) {
    return {a.at(0) + b.at(0), a.at(1) + b.at(1), a.at(2) + b.at(2)};
}

inline std::vector<std::vector<double>> rectangle_coords(
    const std::vector<double>& size,
    const std::vector<double>& position,
    bool has_position)
{
    const size_t dimensions = has_position ? position.size() : size.size();
    std::vector<std::vector<double>> points(4, std::vector<double>(dimensions, 0.0));
    if (has_position) {
        for (auto& point : points) {
            point = position;
        }
    }

    std::vector<size_t> non_empty_coords;
    for (size_t i = 0; i < size.size(); ++i) {
        if (size[i] != 0.0) {
            non_empty_coords.push_back(i);
        }
    }
    points[1][non_empty_coords.at(0)] += size.at(non_empty_coords.at(0));
    for (size_t i = 0; i < size.size(); ++i) {
        points[2][i] += size[i];
    }
    points[3][non_empty_coords.at(1)] += size.at(non_empty_coords.at(1));
    return points;
}

inline IfcUtil::IfcBaseClass* polyline(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& points,
    bool closed = true)
{
    return ifcapi::bindings::shape_builder_polyline(file, points, closed, {}, false, {});
}

inline IfcUtil::IfcBaseClass* rectangle(
    IfcParse::IfcFile* file,
    const std::vector<double>& size,
    const std::vector<double>& position = {},
    bool has_position = false)
{
    return polyline(file, rectangle_coords(size, position, has_position), true);
}

inline IfcUtil::IfcBaseClass* extrude_y(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile_or_curve,
    double magnitude,
    const std::vector<double>& position)
{
    return ifcapi::bindings::shape_builder_extrude(
        file,
        profile_or_curve,
        magnitude,
        position,
        {0.0, 0.0, -1.0},
        {0.0, -1.0, 0.0},
        {1.0, 0.0, 0.0},
        {},
        false);
}

inline IfcUtil::IfcBaseClass* extrude_z(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile_or_curve,
    double magnitude,
    const std::vector<double>& position)
{
    return ifcapi::bindings::shape_builder_extrude(
        file,
        profile_or_curve,
        magnitude,
        position,
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0},
        {},
        false);
}

inline void translate_items(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& items,
    const std::vector<double>& translation)
{
    for (auto* item : items) {
        ifcapi::bindings::shape_builder_translate(file, item, translation, false);
    }
}

inline void append_items(std::vector<IfcUtil::IfcBaseClass*>& target, const std::vector<IfcUtil::IfcBaseClass*>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_SHAPE_BUILDER_H */
