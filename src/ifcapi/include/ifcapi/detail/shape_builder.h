// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_SHAPE_BUILDER_H
#define IFCAPI_DETAIL_SHAPE_BUILDER_H

#include "ifcapi/bindings/shape_builder.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <array>
#include <stdexcept>
#include <variant>
#include <vector>

namespace ifcapi {
namespace detail {

inline std::vector<express::Base> const_refs(const std::vector<express::Base>& items) {
    return items;
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

inline ifcapi::bindings::Vec3 fixed_vec3(const std::vector<double>& values) {
    if (values.size() != 3) throw std::invalid_argument("Expected an XYZ vector");
    return {values[0], values[1], values[2]};
}

inline ifcapi::bindings::Vec2 fixed_vec2(const std::vector<double>& values) {
    if (values.size() != 2) throw std::invalid_argument("Expected an XY vector");
    return {values[0], values[1]};
}

inline ifcapi::bindings::Vec2OrVec3 fixed_vec2_or_vec3(const std::vector<double>& values) {
    if (values.size() == 2) return ifcapi::bindings::Vec2{values[0], values[1]};
    if (values.size() == 3) return ifcapi::bindings::Vec3{values[0], values[1], values[2]};
    throw std::invalid_argument("Expected an XY or XYZ vector");
}

inline ifcapi::bindings::Vec2OrVec3List fixed_vec2_or_vec3_list(
    const std::vector<std::vector<double>>& points)
{
    if (points.empty() || points.front().size() == 2) {
        std::vector<ifcapi::bindings::Vec2> result;
        result.reserve(points.size());
        for (const auto& point : points) {
            if (point.size() != 2) throw std::invalid_argument("Polyline points must all be XY or all be XYZ");
            result.push_back({point[0], point[1]});
        }
        return result;
    }
    if (points.front().size() == 3) {
        std::vector<ifcapi::bindings::Vec3> result;
        result.reserve(points.size());
        for (const auto& point : points) {
            if (point.size() != 3) throw std::invalid_argument("Polyline points must all be XY or all be XYZ");
            result.push_back({point[0], point[1], point[2]});
        }
        return result;
    }
    throw std::invalid_argument("Polyline points must all be XY or all be XYZ");
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

inline express::Base polyline(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    bool closed = true)
{
    std::optional<std::vector<ifcapi::bindings::ShapeBuilderCurveSegment>> segments;
    if (closed && !points.empty()) {
        std::vector<std::uint32_t> indices;
        for (std::uint32_t index = 0; index < points.size(); ++index) indices.push_back(index);
        indices.push_back(0);
        segments = std::vector<ifcapi::bindings::ShapeBuilderCurveSegment>{
            ifcapi::bindings::ShapeBuilderLineSegment{std::move(indices)}};
    }
    return ifcapi::bindings::shape_builder_polyline(
        file,
        ifcapi::bindings::ShapeBuilderPolylineOptions{
            fixed_vec2_or_vec3_list(points), {}, std::move(segments)});
}

inline express::Base rectangle(
    ifcopenshell::file* file,
    const std::vector<double>& size,
    const std::vector<double>& position = {},
    bool has_position = false)
{
    return polyline(file, rectangle_coords(size, position, has_position), true);
}

inline express::Base extrude_y(
    ifcopenshell::file* file,
    express::Base profile_or_curve,
    double magnitude,
    const std::vector<double>& position)
{
    return ifcapi::bindings::shape_builder_extrude(
        file,
        ifcapi::bindings::ShapeBuilderExtrudeOptions{
            profile_or_curve,
            magnitude,
            fixed_vec3(position),
            ifcapi::bindings::Vec3{0.0, 0.0, -1.0},
            ifcapi::bindings::Vec3{0.0, -1.0, 0.0},
            ifcapi::bindings::Vec3{1.0, 0.0, 0.0},
            {}});
}

inline express::Base extrude_z(
    ifcopenshell::file* file,
    express::Base profile_or_curve,
    double magnitude,
    const std::vector<double>& position)
{
    return ifcapi::bindings::shape_builder_extrude(
        file,
        ifcapi::bindings::ShapeBuilderExtrudeOptions{
            profile_or_curve,
            magnitude,
            fixed_vec3(position),
            ifcapi::bindings::Vec3{0.0, 0.0, 1.0},
            ifcapi::bindings::Vec3{0.0, 0.0, 1.0},
            ifcapi::bindings::Vec3{1.0, 0.0, 0.0},
            {}});
}

inline void translate_items(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    const std::vector<double>& translation)
{
    for (auto item : items) {
        ifcapi::bindings::shape_builder_translate(
            file,
            ifcapi::bindings::ShapeBuilderTranslateOptions{
                item, fixed_vec2_or_vec3(translation), false});
    }
}

inline void append_items(std::vector<express::Base>& target, const std::vector<express::Base>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_SHAPE_BUILDER_H */
