// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/shape_builder.h"
#include "ifcapi/detail/vector.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

using Point = std::vector<double>;
using Points = std::vector<Point>;
using ifcapi::bindings::GeometryRailingSupport;
using ifcapi::bindings::GeometryWallMountedHandrailResult;

constexpr double PI = 3.141592653589793238462643383279502884;
constexpr double PRECISION = 1e-5;
constexpr double NUMERIC_EPSILON = 1e-12;
constexpr double ARC_MIDDLE_POINT_COS = 0.707106781186547524400844362104849039;

double mm(double value) { return value / 1000.0; }

struct RailingDims {
    double railing_radius;
    double height_below_handrail;
    double terminal_radius;
    double fillet_radius;
    double support_spacing;
    double support_length;
    double support_arc_radius;
    double support_disk_radius;
    double support_disk_depth;
    double clear_width;
    std::string cap_type;
};

bool finite(double value) { return std::isfinite(value); }

const char* terminal_type_name(ifcapi::bindings::GeometryRailingTerminalType value) {
    using Terminal = ifcapi::bindings::GeometryRailingTerminalType;
    switch (value) {
    case Terminal::RETURN_180: return "180";
    case Terminal::TO_END_POST: return "TO_END_POST";
    case Terminal::TO_WALL: return "TO_WALL";
    case Terminal::TO_FLOOR: return "TO_FLOOR";
    case Terminal::TO_END_POST_AND_FLOOR: return "TO_END_POST_AND_FLOOR";
    case Terminal::NONE: return "NONE";
    }
    throw std::invalid_argument("Unsupported railing terminal type");
}

template <typename T>
bool finite_point(const T& point) {
    return point.size() == 3 && std::all_of(point.begin(), point.end(), [](double value) { return finite(value); });
}

template <typename T>
bool finite_points(const T& points) {
    return std::all_of(points.begin(), points.end(), [](const auto& point) { return finite_point(point); });
}

Points dynamic_points(const std::vector<ifcapi::bindings::Vec3>& points) {
    Points result;
    result.reserve(points.size());
    for (const auto& point : points) result.emplace_back(point.begin(), point.end());
    return result;
}

std::optional<Point> normalized(const Point& value) {
    if (!finite_point(value)) return std::nullopt;
    const double length = ifcapi::detail::vec_norm(value);
    if (!finite(length) || length <= NUMERIC_EPSILON) return std::nullopt;
    auto result = ifcapi::detail::vec_mul(value, 1.0 / length);
    return finite_point(result) ? std::optional<Point>(std::move(result)) : std::nullopt;
}

bool collinear(const Point& d0, const Point& d1) {
    return ifcapi::detail::vec_norm(ifcapi::detail::vec_cross3(d0, d1)) < PRECISION;
}

std::optional<Point> horizontal_orthogonal(const Point& direction) {
    if (direction.size() != 3) return std::nullopt;
    return normalized({direction[1], -direction[0], 0.0});
}

Point cap_orthogonal(const Point& direction) {
    if (auto result = horizontal_orthogonal(direction)) return *result;
    return {1.0, 0.0, 0.0};
}

std::vector<ifcapi::bindings::ShapeBuilderCurveSegment> curve_segments(
    size_t point_count,
    const std::vector<int>& arc_midpoints)
{
    std::vector<ifcapi::bindings::ShapeBuilderCurveSegment> result;
    std::vector<std::uint32_t> current_line;
    size_t index = 0;
    while (index + 1 < point_count) {
        const bool arc = std::find(arc_midpoints.begin(), arc_midpoints.end(), static_cast<int>(index + 1)) !=
            arc_midpoints.end();
        if (arc && index + 2 < point_count) {
            if (!current_line.empty()) {
                result.emplace_back(ifcapi::bindings::ShapeBuilderLineSegment{std::move(current_line)});
                current_line.clear();
            }
            result.emplace_back(ifcapi::bindings::ShapeBuilderArcSegment{{
                static_cast<std::uint32_t>(index),
                static_cast<std::uint32_t>(index + 1),
                static_cast<std::uint32_t>(index + 2)}});
            index += 2;
        } else {
            if (current_line.empty()) current_line.push_back(static_cast<std::uint32_t>(index));
            current_line.push_back(static_cast<std::uint32_t>(index + 1));
            ++index;
        }
    }
    if (!current_line.empty()) result.emplace_back(ifcapi::bindings::ShapeBuilderLineSegment{std::move(current_line)});
    return result;
}

std::optional<Point> line_intersection(
    const Point& p1,
    const Point& p2,
    const Point& p3,
    const Point& p4)
{
    const auto u = ifcapi::detail::vec_sub(p2, p1);
    const auto v = ifcapi::detail::vec_sub(p4, p3);
    const auto w = ifcapi::detail::vec_sub(p1, p3);
    const double uu = ifcapi::detail::vec_dot(u, u);
    const double uv = ifcapi::detail::vec_dot(u, v);
    const double vv = ifcapi::detail::vec_dot(v, v);
    const double uw = ifcapi::detail::vec_dot(u, w);
    const double vw = ifcapi::detail::vec_dot(v, w);
    const double denominator = uu * vv - uv * uv;
    const double scale = std::max(1.0, uu * vv);
    if (!finite(denominator) || std::abs(denominator) <= NUMERIC_EPSILON * scale) return std::nullopt;
    const double s = (uv * vw - vv * uw) / denominator;
    const double t = (uu * vw - uv * uw) / denominator;
    if (!finite(s) || !finite(t)) return std::nullopt;
    auto first = ifcapi::detail::vec_add(p1, ifcapi::detail::vec_mul(u, s));
    auto second = ifcapi::detail::vec_add(p3, ifcapi::detail::vec_mul(v, t));
    if (!finite_point(first) || !finite_point(second) ||
        ifcapi::detail::vec_norm(ifcapi::detail::vec_sub(first, second)) > PRECISION) {
        return std::nullopt;
    }
    return first;
}

std::optional<Points> fillet_points(const Point& v0, const Point& v1, const Point& v2, double radius) {
    const auto dir1 = normalized(ifcapi::detail::vec_sub(v0, v1));
    const auto dir2 = normalized(ifcapi::detail::vec_sub(v2, v1));
    if (!dir1 || !dir2) return std::nullopt;
    const double cosine = std::clamp(ifcapi::detail::vec_dot(*dir1, *dir2), -1.0, 1.0);
    const double edge_angle = std::acos(cosine);
    const double tangent = std::tan(edge_angle / 2.0);
    if (!finite(tangent) || std::abs(tangent) <= NUMERIC_EPSILON) return std::nullopt;
    const double slide_distance = radius / tangent;
    if (!finite(slide_distance)) return std::nullopt;

    auto first = ifcapi::detail::vec_add(v1, ifcapi::detail::vec_mul(*dir1, slide_distance));
    auto last = ifcapi::detail::vec_add(v1, ifcapi::detail::vec_mul(*dir2, slide_distance));
    const auto normal = normalized(ifcapi::detail::vec_cross3(
        ifcapi::detail::vec_sub(v1, v0), ifcapi::detail::vec_sub(v2, v0)));
    if (!normal) return std::nullopt;
    const auto center = line_intersection(
        first,
        ifcapi::detail::vec_add(first, ifcapi::detail::vec_cross3(*normal, *dir1)),
        last,
        ifcapi::detail::vec_add(last, ifcapi::detail::vec_cross3(*normal, *dir2)));
    if (!center) return std::nullopt;
    const auto middle_direction = normalized(ifcapi::detail::vec_sub(ifcapi::detail::np_lerp(first, last, 0.5), *center));
    if (!middle_direction) return std::nullopt;
    auto middle = ifcapi::detail::vec_add(*center, ifcapi::detail::vec_mul(*middle_direction, radius));
    Points result = {std::move(first), std::move(middle), std::move(last)};
    return finite_points(result) ? std::optional<Points>(std::move(result)) : std::nullopt;
}

std::optional<GeometryRailingSupport> make_support(
    const Point& point,
    const Point& railing_direction,
    const RailingDims& dims)
{
    const auto ortho = horizontal_orthogonal(railing_direction);
    if (!ortho) return std::nullopt;
    const Point z_down = {0.0, 0.0, -1.0};
    const auto arc_center = ifcapi::detail::vec_add(point, ifcapi::detail::vec_mul(*ortho, dims.support_length));
    Points support_points = {
        point,
        ifcapi::detail::vec_add(
            ifcapi::detail::vec_sub(
                arc_center,
                ifcapi::detail::vec_mul(*ortho, dims.support_length * std::cos(PI / 4.0))),
            ifcapi::detail::vec_mul(z_down, dims.support_length * std::sin(PI / 4.0))),
        ifcapi::detail::vec_add(arc_center, ifcapi::detail::vec_mul(z_down, dims.support_length)),
    };
    if (!finite_points(support_points)) return std::nullopt;
    const double disk_rotation = std::atan2((*ortho)[0], (*ortho)[1]);
    if (!finite(disk_rotation)) return std::nullopt;
    return GeometryRailingSupport{
        support_points,
        dims.support_arc_radius,
        support_points.back(),
        dims.support_disk_radius,
        dims.support_disk_depth,
        disk_rotation};
}

std::vector<GeometryRailingSupport> collect_supports(
    const Points& coords,
    bool manual_supports,
    const RailingDims& dims)
{
    std::vector<GeometryRailingSupport> supports;
    Points simplified = {coords.front()};
    std::optional<Point> previous_direction = normalized(ifcapi::detail::vec_sub(coords[1], coords[0]));

    for (size_t i = 1; i + 1 < coords.size(); ++i) {
        const auto current_direction = normalized(ifcapi::detail::vec_sub(coords[i + 1], coords[i]));
        if (!current_direction) continue;
        if (!previous_direction) {
            previous_direction = current_direction;
            continue;
        }
        if (!collinear(*current_direction, *previous_direction)) {
            simplified.push_back(coords[i]);
            previous_direction = current_direction;
        } else if (manual_supports) {
            if (auto support = make_support(coords[i], *current_direction, dims)) supports.push_back(std::move(*support));
        }
    }
    simplified.push_back(coords.back());
    if (manual_supports) return supports;

    for (size_t i = 0; i + 1 < simplified.size(); ++i) {
        const auto edge = ifcapi::detail::vec_sub(simplified[i + 1], simplified[i]);
        const double length = ifcapi::detail::vec_norm(edge);
        const auto edge_direction = normalized(edge);
        if (!edge_direction || !finite(length) || !horizontal_orthogonal(edge)) continue;
        const int count = static_cast<int>(std::floor(length / dims.support_spacing)) + 1;
        const double remainder = std::fmod(length, dims.support_spacing);
        const auto start = ifcapi::detail::vec_add(simplified[i], ifcapi::detail::vec_mul(*edge_direction, remainder / 2.0));
        for (int support_index = 0; support_index < count; ++support_index) {
            const auto position = ifcapi::detail::vec_add(
                start, ifcapi::detail::vec_mul(*edge_direction, support_index * dims.support_spacing));
            if (auto support = make_support(position, edge, dims)) supports.push_back(std::move(*support));
        }
    }
    return supports;
}

std::pair<Points, Points> add_turning_fillets(const Points& base_points, const RailingDims& dims, bool looped_path) {
    Points arc_points;
    if (base_points.size() < 3) return {base_points, arc_points};

    Points output = {base_points.front()};
    std::optional<Point> previous_direction = normalized(ifcapi::detail::vec_sub(base_points[1], base_points[0]));
    for (size_t i = 1; i + 1 < base_points.size(); ++i) {
        const auto current_direction = normalized(ifcapi::detail::vec_sub(base_points[i + 1], base_points[i]));
        if (!current_direction) {
            output.push_back(base_points[i]);
            continue;
        }
        if (!previous_direction || collinear(*current_direction, *previous_direction)) {
            output.push_back(base_points[i]);
        } else if (auto fillet = fillet_points(base_points[i - 1], base_points[i], base_points[i + 1], dims.fillet_radius)) {
            output.insert(output.end(), fillet->begin(), fillet->end());
            arc_points.push_back((*fillet)[1]);
        } else {
            output.push_back(base_points[i]);
        }
        previous_direction = current_direction;
    }
    if (looped_path) {
        output.front() = output.back();
    } else {
        output.push_back(base_points.back());
    }
    return {std::move(output), std::move(arc_points)};
}

std::optional<Point> terminal_direction(const Points& coords) {
    if (coords.size() < 2) return std::nullopt;
    for (size_t offset = 1; offset < coords.size(); ++offset) {
        if (auto direction = normalized(ifcapi::detail::vec_sub(coords.back(), coords[coords.size() - 1 - offset]))) {
            return direction;
        }
    }
    return std::nullopt;
}

void add_cap(Points& railing_coords, Points& arc_points, bool start, const RailingDims& dims) {
    if (dims.cap_type == "NONE") return;
    Points coords = railing_coords;
    Points arcs = arc_points;
    if (start) {
        std::reverse(coords.begin(), coords.end());
        std::reverse(arcs.begin(), arcs.end());
    }
    const auto direction = terminal_direction(coords);
    if (!direction) return;
    const Point start_point = coords.back();
    Point ortho = cap_orthogonal(*direction);
    Point local_z_down = ifcapi::detail::vec_cross3(*direction, ortho);
    if (start) ortho = ifcapi::detail::vec_mul(ortho, -1.0);
    const Point z_down = {0.0, 0.0, -1.0};
    Points cap_coords;

    if (dims.cap_type == "180" || dims.cap_type == "TO_END_POST") {
        auto arc_point = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(*direction, dims.terminal_radius)),
            ifcapi::detail::vec_mul(local_z_down, dims.terminal_radius));
        arcs.push_back(arc_point);
        cap_coords = {arc_point, ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(local_z_down, 2.0 * dims.terminal_radius))};
        if (dims.cap_type == "TO_END_POST") {
            auto end_point = coords[coords.size() - 2];
            end_point[2] -= 2.0 * dims.terminal_radius;
            cap_coords.push_back(std::move(end_point));
        }
    } else if (dims.cap_type == "TO_WALL") {
        auto arc_point = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(*direction, dims.clear_width * ARC_MIDDLE_POINT_COS)),
            ifcapi::detail::vec_mul(ortho, dims.clear_width * (1.0 - ARC_MIDDLE_POINT_COS)));
        arcs.push_back(arc_point);
        cap_coords = {arc_point, ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(ortho, dims.clear_width)),
            ifcapi::detail::vec_mul(*direction, dims.clear_width))};
    } else if (dims.cap_type == "TO_FLOOR") {
        auto arc_point = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(*direction, dims.terminal_radius * ARC_MIDDLE_POINT_COS)),
            ifcapi::detail::vec_mul(z_down, dims.terminal_radius * (1.0 - ARC_MIDDLE_POINT_COS)));
        auto arc_end = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(*direction, dims.terminal_radius)),
            ifcapi::detail::vec_mul(z_down, dims.terminal_radius));
        arcs.push_back(arc_point);
        cap_coords = {arc_point, arc_end, ifcapi::detail::vec_add(
            arc_end, ifcapi::detail::vec_mul(z_down, dims.height_below_handrail - dims.terminal_radius))};
    } else if (dims.cap_type == "TO_END_POST_AND_FLOOR") {
        auto first_arc_end = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(*direction, dims.terminal_radius)),
            ifcapi::detail::vec_mul(local_z_down, dims.terminal_radius));
        auto end_point = coords[coords.size() - 2];
        end_point[2] -= dims.height_below_handrail;
        auto first = fillet_points(
            start_point,
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(*direction, dims.terminal_radius)),
            first_arc_end,
            dims.terminal_radius);
        auto second = fillet_points(
            first_arc_end,
            ifcapi::detail::vec_add(first_arc_end, ifcapi::detail::vec_mul(local_z_down, dims.terminal_radius)),
            end_point,
            dims.terminal_radius);
        if (first && second) {
            cap_coords = {start_point};
            cap_coords.insert(cap_coords.end(), first->begin(), first->end());
            cap_coords.insert(cap_coords.end(), second->begin(), second->end());
            cap_coords.push_back(end_point);
            arcs.push_back((*first)[1]);
            arcs.push_back((*second)[1]);
        } else {
            cap_coords = {first_arc_end, end_point};
        }
    }

    if (!finite_points(cap_coords)) return;
    coords.insert(coords.end(), cap_coords.begin(), cap_coords.end());
    if (start) {
        std::reverse(coords.begin(), coords.end());
        std::reverse(arcs.begin(), arcs.end());
    }
    railing_coords = std::move(coords);
    arc_points = std::move(arcs);
}

std::vector<int> arc_indices(const Points& points, const Points& arc_points) {
    std::vector<int> result;
    size_t start = 0;
    for (const auto& arc_point : arc_points) {
        bool found = false;
        for (size_t i = start; i < points.size(); ++i) {
            if (ifcapi::detail::np_allclose(arc_point, points[i])) {
                result.push_back(static_cast<int>(i));
                start = i + 1;
                found = true;
                break;
            }
        }
        if (!found) throw std::runtime_error("Arc midpoint is not present in the handrail polyline");
    }
    return result;
}

void validate_compute_options(
    const ifcapi::bindings::GeometryComputeWallMountedHandrailOptions& options,
    bool manual_supports,
    const std::string& terminal_type,
    double unit_scale)
{
    static const std::array<const char*, 6> terminals = {
        "180", "TO_END_POST", "TO_WALL", "TO_FLOOR", "TO_END_POST_AND_FLOOR", "NONE"};
    if (options.railing_path.size() < 2) throw std::invalid_argument("Railing path requires at least two points");
    if (!finite_points(options.railing_path)) throw std::invalid_argument("Railing path must contain finite XYZ points");
    if (std::find(terminals.begin(), terminals.end(), terminal_type) == terminals.end()) {
        throw std::invalid_argument("Unsupported railing terminal type: " + terminal_type);
    }
    if (!finite(options.railing_diameter) || options.railing_diameter <= 0.0) {
        throw std::invalid_argument("Railing diameter must be positive");
    }
    if (!finite(options.clear_width) || options.clear_width <= 0.0) {
        throw std::invalid_argument("Railing clear width must be positive");
    }
    if (!finite(unit_scale) || unit_scale <= 0.0) throw std::invalid_argument("Unit scale must be positive");
    const double radius = options.railing_diameter / 2.0;
    if (!finite(options.height) || options.height < radius) {
        throw std::invalid_argument("Railing height must be at least the handrail radius");
    }
    if (!finite(options.support_spacing)) throw std::invalid_argument("Support spacing must be finite");
    if (!manual_supports && options.support_spacing <= 0.0) {
        throw std::invalid_argument("Automatic support spacing must be positive");
    }
}

express::Base require_entity(express::Base value, const char* operation) {
    if (value) return value;
    const char* detail = ifcopenshell_last_error_message();
    throw std::runtime_error(detail && *detail ? detail : operation);
}

express::Base materialize_support(ifcopenshell::file* file, const GeometryRailingSupport& support) {
    auto polyline = require_entity(
        ifcapi::bindings::shape_builder_polyline(
            file,
            ifcapi::bindings::ShapeBuilderPolylineOptions{
                ifcapi::detail::fixed_vec2_or_vec3_list(support.arc_polyline),
                {},
                curve_segments(support.arc_polyline.size(), {1})}),
        "Failed to create railing support polyline");
    return require_entity(
        ifcapi::bindings::shape_builder_swept_disk_solid(file, &polyline, support.arc_radius),
        "Failed to create railing support swept disk");
}

express::Base materialize_disk(ifcopenshell::file* file, const GeometryRailingSupport& support) {
    auto circle = require_entity(
        ifcapi::bindings::shape_builder_circle(file, {}, support.disk_radius),
        "Failed to create railing support disk circle");
    const double rotated = -support.disk_z_rotation;
    return require_entity(
        ifcapi::bindings::shape_builder_extrude(
            file,
            ifcapi::bindings::ShapeBuilderExtrudeOptions{
                circle,
                support.disk_depth,
                ifcapi::detail::fixed_vec3(support.disk_position),
                ifcapi::bindings::Vec3{0.0, 0.0, -1.0},
                ifcapi::detail::fixed_vec3(ifcapi::detail::rotate_xy({0.0, -1.0, 0.0}, rotated)),
                ifcapi::detail::fixed_vec3(ifcapi::detail::rotate_xy({1.0, 0.0, 0.0}, rotated)),
                {}}),
        "Failed to create railing support attachment disk");
}

} // namespace

namespace ifcapi {
namespace bindings {

GeometryWallMountedHandrailResult geometry_compute_wall_mounted_handrail_geometry(
    const GeometryComputeWallMountedHandrailOptions& options)
{
    const bool manual_supports = options.use_manual_supports.value_or(false);
    const bool looped_path = options.looped_path.value_or(false);
    const std::string terminal_type = terminal_type_name(
        options.terminal_type.value_or(GeometryRailingTerminalType::RETURN_180));
    const double unit_scale = options.unit_scale.value_or(1.0);
    validate_compute_options(options, manual_supports, terminal_type, unit_scale);

    const double railing_radius = options.railing_diameter / 2.0;
    const RailingDims dims{
        railing_radius,
        options.height - railing_radius,
        mm(150.0) / unit_scale,
        mm(100.0) / unit_scale,
        options.support_spacing,
        options.clear_width + railing_radius,
        mm(10.0) / unit_scale,
        railing_radius,
        mm(20.0) / unit_scale,
        options.clear_width,
        terminal_type};

    Points railing_coords = dynamic_points(options.railing_path);
    for (auto& point : railing_coords) point[2] += railing_radius;
    if (looped_path && railing_coords.size() > 2 &&
        ifcapi::detail::np_allclose(railing_coords.front(), railing_coords.back())) {
        railing_coords.pop_back();
    }
    if (looped_path) {
        railing_coords.push_back(railing_coords[0]);
        railing_coords.push_back(railing_coords[1]);
    }

    auto supports = collect_supports(railing_coords, manual_supports, dims);
    auto [polyline, arc_points] = add_turning_fillets(railing_coords, dims, looped_path);
    if (!looped_path) {
        add_cap(polyline, arc_points, true, dims);
        add_cap(polyline, arc_points, false, dims);
    }
    if (!finite_points(polyline)) throw std::runtime_error("Railing computation produced non-finite coordinates");
    return GeometryWallMountedHandrailResult{
        polyline, arc_indices(polyline, arc_points), railing_radius, supports};
}

express::Base geometry_add_railing_representation(
    ifcopenshell::file* file,
    const GeometryAddRailingRepresentationOptions& options)
{
    if (!file || !options.context) throw std::invalid_argument("Railing representation requires a file and context");
    if (options.context.file() != file) throw std::invalid_argument("Railing context must belong to the supplied file");

    const double unit_scale = options.unit_scale.value_or(unit_calculate_unit_scale(file, "LENGTHUNIT"));
    const auto default_dimension = [unit_scale](double millimetres) { return mm(millimetres) / unit_scale; };
    const auto path = options.railing_path.value_or(std::vector<Vec3>{
        Vec3{0.0, 0.0, 1.0 / unit_scale},
        Vec3{1.0 / unit_scale, 0.0, 1.0 / unit_scale},
        Vec3{2.0 / unit_scale, 0.0, 1.0 / unit_scale}});
    GeometryComputeWallMountedHandrailOptions compute_options{
        path,
        options.support_spacing.value_or(default_dimension(1000.0)),
        options.railing_diameter.value_or(default_dimension(50.0)),
        options.clear_width.value_or(default_dimension(40.0)),
        options.height.value_or(default_dimension(1000.0)),
        options.use_manual_supports,
        options.terminal_type,
        options.looped_path,
        unit_scale};

    auto geometry = geometry_compute_wall_mounted_handrail_geometry(compute_options);
    if (file->schema() && file->schema()->name() == "IFC2X3" &&
        (!geometry.supports.empty() || !geometry.handrail_arc_point_indices.empty())) {
        throw std::invalid_argument("Railing arcs are not supported for IFC2X3");
    }

    std::vector<express::Base> items;
    items.reserve(geometry.supports.size() * 2 + 1);
    for (const auto& support : geometry.supports) {
        items.push_back(materialize_support(file, support));
        items.push_back(materialize_disk(file, support));
    }
    auto handrail_path = require_entity(
        shape_builder_polyline(
            file,
            ShapeBuilderPolylineOptions{
                ifcapi::detail::fixed_vec2_or_vec3_list(geometry.handrail_polyline),
                {},
                curve_segments(geometry.handrail_polyline.size(), geometry.handrail_arc_point_indices)}),
        "Failed to create handrail polyline");
    items.push_back(require_entity(
        shape_builder_swept_disk_solid(file, &handrail_path, geometry.handrail_radius),
        "Failed to create handrail swept disk"));
    return require_entity(
        shape_builder_representation(
            file, ShapeBuilderRepresentationOptions{options.context, ifcapi::detail::const_refs(items), "SolidModel"}),
        "Failed to create railing representation");
}

} // namespace bindings
} // namespace ifcapi
