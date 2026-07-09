// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/shape_builder.h"
#include "ifcapi/detail/vector.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

constexpr double PI = 3.141592653589793238462643383279502884;

inline double mm(double value) { return value / 1000.0; }
inline double to_unit(double value, double unit_scale) { return value / unit_scale; }
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }

std::vector<double> xy_yx_orthogonal(const std::vector<double>& direction) {
    return ifcapi::detail::np_normalized(ifcapi::detail::np_to_3d({direction.at(1), -direction.at(0)}));
}

express::Base extrude_support_disk(
    ifcopenshell::file* file,
    express::Base circle,
    double depth,
    const std::vector<double>& position,
    double angle)
{
    const double rotated = -angle;
    return ifcapi::bindings::shape_builder_extrude(
        file,
        ifcapi::bindings::ShapeBuilderExtrudeOptions{
            circle,
            depth,
            position,
            {0.0, 0.0, -1.0},
            ifcapi::detail::rotate_xy({0.0, -1.0, 0.0}, rotated),
            ifcapi::detail::rotate_xy({1.0, 0.0, 0.0}, rotated),
            {}});
}

std::vector<express::Base> add_support_on_point(
    ifcopenshell::file* file,
    const std::vector<double>& point,
    const std::vector<double>& railing_direction,
    double support_length,
    double support_radius,
    double support_disk_radius,
    double support_disk_depth)
{
    const auto z_down = ifcapi::detail::v3(0.0, 0.0, -1.0);
    const auto ortho_dir = xy_yx_orthogonal(railing_direction);
    const auto arc_center = ifcapi::detail::vec_add(point, ifcapi::detail::vec_mul(ortho_dir, support_length));
    std::vector<std::vector<double>> support_points = {
        point,
        ifcapi::detail::vec_add(
            ifcapi::detail::vec_sub(arc_center, ifcapi::detail::vec_mul(ortho_dir, support_length * std::cos(PI / 4.0))),
            ifcapi::detail::vec_mul(z_down, support_length * std::sin(PI / 4.0))),
        ifcapi::detail::vec_add(arc_center, ifcapi::detail::vec_mul(z_down, support_length)),
    };
    auto polyline = ifcapi::bindings::shape_builder_polyline(
        file,
        ifcapi::bindings::ShapeBuilderPolylineOptions{support_points, false, {}, {1}});
    auto solid = ifcapi::bindings::shape_builder_swept_disk_solid(file, &polyline, support_radius);
    auto disk_circle = ifcapi::bindings::shape_builder_circle(file, {}, support_disk_radius);
    const double angle = ifcapi::detail::np_angle_signed({0.0, 1.0}, {ortho_dir[0], ortho_dir[1]});
    auto disk = extrude_support_disk(file, disk_circle, support_disk_depth, support_points.back(), angle);
    return {solid, disk};
}

std::vector<double> get_fillet_points_midpoint(
    const std::vector<double>& v0,
    const std::vector<double>& v1,
    const std::vector<double>& v2,
    double radius,
    std::vector<double>& out_first,
    std::vector<double>& out_last)
{
    const auto dir1 = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(v0, v1));
    const auto dir2 = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(v2, v1));
    const double edge_angle = ifcapi::detail::np_angle(dir1, dir2);
    const double slide_distance = radius / std::tan(edge_angle / 2.0);
    out_first = ifcapi::detail::vec_add(v1, ifcapi::detail::vec_mul(dir1, slide_distance));
    out_last = ifcapi::detail::vec_add(v1, ifcapi::detail::vec_mul(dir2, slide_distance));
    const auto normal = ifcapi::detail::np_normal({v0, v1, v2});
    const auto center = ifcapi::detail::np_intersect_line_line(
        out_first,
        ifcapi::detail::vec_add(out_first, ifcapi::detail::vec_cross3(normal, dir1)),
        out_last,
        ifcapi::detail::vec_add(out_last, ifcapi::detail::vec_cross3(normal, dir2)))[0];
    const auto dir = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(ifcapi::detail::np_lerp(out_first, out_last, 0.5), center));
    return ifcapi::detail::vec_add(center, ifcapi::detail::vec_mul(dir, radius));
}

std::vector<std::vector<double>> get_fillet_points(
    const std::vector<double>& v0,
    const std::vector<double>& v1,
    const std::vector<double>& v2,
    double radius)
{
    std::vector<double> first;
    std::vector<double> last;
    auto middle = get_fillet_points_midpoint(v0, v1, v2, radius, first, last);
    return {first, middle, last};
}

bool collinear(const std::vector<double>& d0, const std::vector<double>& d1) {
    return ifcapi::detail::is_x(ifcapi::detail::np_angle(d0, d1), 0.0);
}

std::vector<std::vector<double>> add_arcs_on_turning_points(
    const std::vector<std::vector<double>>& base_points,
    std::vector<std::vector<double>>& arc_points,
    double railing_fillet_radius,
    bool looped_path)
{
    if (base_points.size() < 3) {
        return base_points;
    }
    std::vector<std::vector<double>> output = {base_points.front()};
    auto prev_dir = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(base_points[1], base_points[0]));
    size_t i = 1;
    while (i < base_points.size() - 1) {
        auto cur_dir = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(base_points[i + 1], base_points[i]));
        if (collinear(cur_dir, prev_dir)) {
            output.push_back(base_points[i]);
        } else {
            auto fillet_points = get_fillet_points(base_points[i - 1], base_points[i], base_points[i + 1], railing_fillet_radius);
            output.insert(output.end(), fillet_points.begin(), fillet_points.end());
            arc_points.push_back(fillet_points[1]);
        }
        prev_dir = cur_dir;
        ++i;
    }
    if (looped_path) {
        output[0] = output.back();
    } else {
        output.push_back(base_points.back());
    }
    return output;
}

std::vector<express::Base> create_supports_items(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& railing_coords,
    bool manual_supports,
    double support_spacing,
    double support_length,
    double support_radius,
    double support_disk_radius,
    double support_disk_depth)
{
    std::vector<express::Base> supports;
    std::vector<std::vector<double>> simplified = {railing_coords.front()};
    auto prev_dir = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(railing_coords[1], railing_coords[0]));
    for (size_t i = 1; i < railing_coords.size() - 1; ++i) {
        auto cur_dir = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(railing_coords[i + 1], railing_coords[i]));
        if (!collinear(cur_dir, prev_dir)) {
            simplified.push_back(railing_coords[i]);
            prev_dir = cur_dir;
        } else if (manual_supports) {
            ifcapi::detail::append_items(
                supports,
                add_support_on_point(file, railing_coords[i], cur_dir, support_length, support_radius, support_disk_radius, support_disk_depth));
        }
    }
    simplified.push_back(railing_coords.back());
    if (manual_supports) {
        return supports;
    }

    for (size_t i = 0; i + 1 < simplified.size(); ++i) {
        const auto edge = ifcapi::detail::vec_sub(simplified[i + 1], simplified[i]);
        const double length = ifcapi::detail::vec_norm(edge);
        const auto edge_dir = ifcapi::detail::np_normalized(edge);
        double n_supports_d = 0.0;
        double support_offset = std::fmod(length, support_spacing);
        n_supports_d = std::floor(length / support_spacing);
        const int n_supports = static_cast<int>(n_supports_d) + 1;
        support_offset /= 2.0;
        const auto start_position = ifcapi::detail::vec_add(simplified[i], ifcapi::detail::vec_mul(edge_dir, support_offset));
        for (int support_i = 0; support_i < n_supports; ++support_i) {
            const auto support_position = ifcapi::detail::vec_add(start_position, ifcapi::detail::vec_mul(edge_dir, support_i * support_spacing));
            ifcapi::detail::append_items(
                supports,
                add_support_on_point(file, support_position, edge, support_length, support_radius, support_disk_radius, support_disk_depth));
        }
    }
    return supports;
}

void add_cap(
    std::vector<std::vector<double>>& railing_coords,
    std::vector<std::vector<double>>& arc_points,
    const std::string& cap_type,
    double terminal_radius,
    double clear_width,
    double height,
    bool start)
{
    auto coords = railing_coords;
    auto arcs = arc_points;
    if (start) {
        std::reverse(coords.begin(), coords.end());
        std::reverse(arcs.begin(), arcs.end());
    }
    auto start_point = coords.back();
    auto cap_dir = ifcapi::detail::np_normalized(ifcapi::detail::vec_sub(coords.back(), coords[coords.size() - 2]));
    auto ortho_dir = xy_yx_orthogonal(cap_dir);
    auto local_z_down = ifcapi::detail::vec_cross3(cap_dir, ortho_dir);
    if (start) {
        ortho_dir = ifcapi::detail::vec_mul(ortho_dir, -1.0);
    }

    const double arc_middle_point_cos = std::sin(45.0 * PI / 180.0);
    std::vector<std::vector<double>> cap_coords;
    if (cap_type == "180" || cap_type == "TO_END_POST") {
        auto arc_point = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(cap_dir, terminal_radius)),
            ifcapi::detail::vec_mul(local_z_down, terminal_radius));
        arcs.push_back(arc_point);
        cap_coords = {arc_point, ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(local_z_down, terminal_radius * 2.0))};
        if (cap_type == "TO_END_POST") {
            auto end_point = coords[coords.size() - 2];
            end_point[2] -= terminal_radius * 2.0;
            cap_coords.push_back(end_point);
        }
    } else if (cap_type == "TO_WALL") {
        auto arc_point = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(cap_dir, clear_width * arc_middle_point_cos)),
            ifcapi::detail::vec_mul(ortho_dir, clear_width * (1.0 - arc_middle_point_cos)));
        arcs.push_back(arc_point);
        cap_coords = {arc_point, ifcapi::detail::vec_add(ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(ortho_dir, clear_width)), ifcapi::detail::vec_mul(cap_dir, clear_width))};
    } else if (cap_type == "TO_FLOOR") {
        const auto z_down = ifcapi::detail::v3(0.0, 0.0, -1.0);
        auto arc_point = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(cap_dir, terminal_radius * arc_middle_point_cos)),
            ifcapi::detail::vec_mul(z_down, terminal_radius * (1.0 - arc_middle_point_cos)));
        arcs.push_back(arc_point);
        auto arc_end = ifcapi::detail::vec_add(ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(cap_dir, terminal_radius)), ifcapi::detail::vec_mul(z_down, terminal_radius));
        cap_coords = {arc_point, arc_end, ifcapi::detail::vec_add(arc_end, ifcapi::detail::vec_mul(z_down, height - terminal_radius))};
    } else if (cap_type == "TO_END_POST_AND_FLOOR") {
        auto first_arc_end = ifcapi::detail::vec_add(
            ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(cap_dir, terminal_radius)),
            ifcapi::detail::vec_mul(local_z_down, terminal_radius));
        auto first_arc = get_fillet_points(start_point, ifcapi::detail::vec_add(start_point, ifcapi::detail::vec_mul(cap_dir, terminal_radius)), first_arc_end, terminal_radius);
        arcs.push_back(first_arc[1]);
        auto end_point = coords[coords.size() - 2];
        end_point[2] -= height;
        auto second_arc = get_fillet_points(first_arc_end, ifcapi::detail::vec_add(first_arc_end, ifcapi::detail::vec_mul(local_z_down, terminal_radius)), end_point, terminal_radius);
        arcs.push_back(second_arc[1]);
        cap_coords = {start_point};
        cap_coords.insert(cap_coords.end(), first_arc.begin(), first_arc.end());
        cap_coords.insert(cap_coords.end(), second_arc.begin(), second_arc.end());
        cap_coords.push_back(end_point);
    } else if (cap_type != "NONE") {
        throw std::runtime_error("Unsupported railing terminal type");
    }

    coords.insert(coords.end(), cap_coords.begin(), cap_coords.end());
    if (start) {
        std::reverse(coords.begin(), coords.end());
        std::reverse(arcs.begin(), arcs.end());
    }
    railing_coords = coords;
    arc_points = arcs;
}

std::vector<int> get_arc_indices(
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<double>>& arc_points)
{
    std::vector<int> arc_indices;
    size_t i_base = 0;
    size_t start = 0;
    for (const auto& arc_point : arc_points) {
        bool found = false;
        for (size_t i = start; i < points.size(); ++i) {
            if (ifcapi::detail::vec_allclose(arc_point, points[i])) {
                const size_t current = i;
                arc_indices.push_back(static_cast<int>(current));
                i_base = current + 1;
                start = i + 1;
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error("Arc point is not present in railing points");
        }
        (void)i_base;
    }
    return arc_indices;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base geometry_add_railing_representation(
    ifcopenshell::file* file,
    const GeometryAddRailingRepresentationOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !options.context) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        express::Base context_value = options.context;
        express::Base* context = &context_value;
        const auto& input_railing_path = options.railing_path;
        const bool use_manual_supports = options.use_manual_supports;
        const double support_spacing = options.support_spacing;
        const double railing_diameter = options.railing_diameter;
        const double clear_width = options.clear_width;
        const std::string& terminal_type = options.terminal_type;
        const double input_height = options.height;
        const bool looped_path = options.looped_path;
        const double unit_scale = options.unit_scale;
        if (input_railing_path.size() < 2) {
            throw std::runtime_error("Railing path requires at least two points");
        }
        std::vector<std::vector<double>> arc_points;
        std::vector<express::Base> items;
        const auto z_down = ifcapi::detail::v3(0.0, 0.0, -1.0);
        const double railing_radius = railing_diameter / 2.0;
        const double height = input_height - railing_radius;
        const double terminal_radius = to_unit(mm(150), unit_scale);
        const double railing_fillet_radius = to_unit(mm(100), unit_scale);
        const double support_length = clear_width + railing_radius;
        const double support_radius = to_unit(mm(10), unit_scale);
        const double support_disk_radius = railing_radius;
        const double support_disk_depth = to_unit(mm(20), unit_scale);

        std::vector<std::vector<double>> railing_coords;
        railing_coords.reserve(input_railing_path.size() + 2);
        for (const auto& point : input_railing_path) {
            railing_coords.push_back(ifcapi::detail::vec_sub(point, ifcapi::detail::vec_mul(z_down, railing_radius)));
        }
        if (looped_path) {
            railing_coords.push_back(railing_coords[0]);
            railing_coords.push_back(railing_coords[1]);
        }

        ifcapi::detail::append_items(
            items,
            create_supports_items(file, railing_coords, use_manual_supports, support_spacing, support_length, support_radius, support_disk_radius, support_disk_depth));
        railing_coords = add_arcs_on_turning_points(railing_coords, arc_points, railing_fillet_radius, looped_path);
        if (!looped_path && terminal_type != "NONE") {
            add_cap(railing_coords, arc_points, terminal_type, terminal_radius, clear_width, height, true);
            add_cap(railing_coords, arc_points, terminal_type, terminal_radius, clear_width, height, false);
        }
        auto path = ifcapi::bindings::shape_builder_polyline(
            file,
            ifcapi::bindings::ShapeBuilderPolylineOptions{
                railing_coords, false, {}, get_arc_indices(railing_coords, arc_points)});
        items.push_back(ifcapi::bindings::shape_builder_swept_disk_solid(file, &path, railing_radius));
        return shape_builder_representation(
            file,
            ShapeBuilderRepresentationOptions{*context, ifcapi::detail::const_refs(items), "SolidModel"});
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
