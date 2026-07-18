// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_WINDOW_BUILDER_H
#define IFCAPI_DETAIL_WINDOW_BUILDER_H

#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/shape_builder.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <algorithm>
#include <vector>

namespace ifcapi {
namespace detail {

inline bool window_l_shape_check(
    double lining_to_panel_offset_y_full,
    double lining_depth,
    const std::vector<double>& lining_to_panel_offset_x,
    const std::vector<double>& lining_thickness)
{
    for (size_t i = 0; i < lining_thickness.size(); ++i) {
        if (lining_to_panel_offset_y_full < lining_depth && lining_to_panel_offset_x.at(i) < lining_thickness.at(i)) {
            return true;
        }
    }
    return false;
}

inline std::vector<std::vector<int>> segments_from_thickness(const std::vector<double>& thickness) {
    std::vector<std::vector<int>> segments;
    std::vector<int> current;
    for (size_t i = 0; i < thickness.size(); ++i) {
        if (thickness[i] == 0.0) {
            if (!current.empty()) {
                segments.push_back(current);
            }
            current.clear();
        } else {
            current.push_back(static_cast<int>(i));
        }
    }
    if (!current.empty()) {
        if (!segments.empty() && segments[0][0] == 0) {
            current.insert(current.end(), segments[0].begin(), segments[0].end());
            segments[0] = current;
        } else {
            segments.push_back(current);
        }
    }
    return segments;
}

inline std::vector<std::vector<double>> window_segment_points(
    const std::vector<int>& segment,
    const std::vector<double>& size,
    const std::vector<double>& thickness)
{
    const double th_left = thickness[0];
    const double th_up = thickness[1];
    const double th_right = thickness[2];
    const double th_bottom = thickness[3];
    std::vector<std::vector<std::vector<double>>> outer = {
        {v2(0.0, 0.0), v2(0.0, size[2])},
        {v2(0.0, size[2]), v2(size[0], size[2])},
        {v2(size[0], size[2]), v2(size[0], 0.0)},
        {v2(size[0], 0.0), v2(0.0, 0.0)},
    };
    std::vector<std::vector<std::vector<double>>> inner = {
        {v2(th_left, th_bottom), v2(th_left, size[2] - th_up)},
        {v2(th_left, size[2] - th_up), v2(size[0] - th_right, size[2] - th_up)},
        {v2(size[0] - th_right, size[2] - th_up), v2(size[0] - th_right, th_bottom)},
        {v2(size[0] - th_right, th_bottom), v2(th_left, th_bottom)},
    };

    std::vector<std::vector<double>> points;
    for (size_t i = 0; i < segment.size(); ++i) {
        const int side = segment[i];
        if (i == 0) {
            points.push_back(outer[side][0]);
        }
        points.push_back(outer[side][1]);
    }
    for (auto it = segment.rbegin(); it != segment.rend(); ++it) {
        if (it == segment.rbegin()) {
            points.push_back(inner[*it][1]);
        }
        points.push_back(inner[*it][0]);
    }
    return points;
}

inline std::vector<express::Base> create_window_frame_simple(
    ifcopenshell::file* file,
    const std::vector<double>& size,
    std::vector<double> thickness,
    const std::vector<double>& position)
{
    std::vector<express::Base> result;
    if (std::count(thickness.begin(), thickness.end(), 0.0) == 0) {
        auto panel_rect = rectangle(file, {size[0], size[2]});
        auto inner_rect = rectangle(
            file,
            {size[0] - (thickness[0] + thickness[2]), size[2] - (thickness[3] + thickness[1])},
            {thickness[0], thickness[3]},
            true);
        auto panel_profile = ifcapi::bindings::shape_builder_profile(
            file,
            ifcapi::bindings::ShapeBuilderProfileOptions{
                panel_rect, {}, std::vector<express::Base>{inner_rect}, "AREA"});
        result.push_back(extrude_y(file, panel_profile, size[1], position));
        return result;
    }

    for (const auto& segment : segments_from_thickness(thickness)) {
        auto curve = polyline(file, window_segment_points(segment, size, thickness), true);
        auto panel_profile = ifcapi::bindings::shape_builder_profile(
            file,
            ifcapi::bindings::ShapeBuilderProfileOptions{curve, {}, {}, "AREA"});
        result.push_back(extrude_y(file, panel_profile, size[1], position));
    }
    return result;
}

inline std::vector<express::Base> create_window_frame_simple(
    ifcopenshell::file* file,
    const std::vector<double>& size,
    double thickness,
    const std::vector<double>& position)
{
    return create_window_frame_simple(file, size, {thickness, thickness, thickness, thickness}, position);
}

struct WindowItems {
    std::vector<express::Base> lining;
    std::vector<express::Base> framing;
    std::vector<express::Base> glazing;
};

inline WindowItems create_ifc_window(
    ifcopenshell::file* file,
    const std::vector<double>& lining_size,
    const std::vector<double>& lining_thickness,
    double lining_to_panel_offset_x,
    double lining_to_panel_offset_y_full,
    const std::vector<double>& frame_size,
    double frame_thickness,
    double glass_thickness,
    const std::vector<double>& position,
    const std::vector<double>& x_offsets)
{
    WindowItems output;
    std::vector<double> main_lining_size = lining_size;
    if (window_l_shape_check(lining_to_panel_offset_y_full, lining_size[1], x_offsets, lining_thickness)) {
        main_lining_size[1] = lining_to_panel_offset_y_full;
        std::vector<double> second_lining_size = lining_size;
        second_lining_size[1] = lining_size[1] - lining_to_panel_offset_y_full;
        std::vector<double> second_lining_thickness;
        for (size_t i = 0; i < lining_thickness.size(); ++i) {
            second_lining_thickness.push_back(std::min(lining_thickness[i], x_offsets[i]));
        }
        append_items(
            output.lining,
            create_window_frame_simple(file, second_lining_size, second_lining_thickness, v3(0.0, lining_to_panel_offset_y_full, 0.0)));
    }

    append_items(output.lining, create_window_frame_simple(file, main_lining_size, lining_thickness, v3(0.0, 0.0, 0.0)));

    auto frame_position = v3(x_offsets[0], lining_to_panel_offset_y_full, x_offsets[3]);
    output.framing = create_window_frame_simple(file, frame_size, frame_thickness, frame_position);

    auto glass_position = add3(frame_position, v3(0.0, frame_size[1] / 2.0 - glass_thickness / 2.0, 0.0));
    auto swept_area = read_ref_attr(output.framing.at(0), "SweptArea");
    auto inner_curves = read_ref_aggregate(swept_area, "InnerCurves");
    auto inner_curve = inner_curves.at(0);
    auto glass_rect = ifcapi::bindings::shape_builder_deep_copy(file, &inner_curve);
    output.glazing.push_back(extrude_y(file, glass_rect, glass_thickness, glass_position));

    std::vector<express::Base> all_items;
    append_items(all_items, output.lining);
    append_items(all_items, output.framing);
    append_items(all_items, output.glazing);
    translate_items(file, all_items, position);
    return output;
}

inline WindowItems create_ifc_window(
    ifcopenshell::file* file,
    const std::vector<double>& lining_size,
    const std::vector<double>& lining_thickness,
    double lining_to_panel_offset_x,
    double lining_to_panel_offset_y_full,
    const std::vector<double>& frame_size,
    double frame_thickness,
    double glass_thickness,
    const std::vector<double>& position)
{
    return create_ifc_window(
        file,
        lining_size,
        lining_thickness,
        lining_to_panel_offset_x,
        lining_to_panel_offset_y_full,
        frame_size,
        frame_thickness,
        glass_thickness,
        position,
        {lining_to_panel_offset_x, lining_to_panel_offset_x, lining_to_panel_offset_x, lining_to_panel_offset_x});
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_WINDOW_BUILDER_H */
