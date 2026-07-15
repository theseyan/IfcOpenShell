// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/shape_builder.h"
#include "ifcapi/detail/window_builder.h"

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

struct DoorLiningProperties {
    double lining_depth;
    double lining_thickness;
    double lining_offset;
    double lining_to_panel_offset_x;
    double lining_to_panel_offset_y;
    double transom_thickness;
    double transom_offset;
    double casing_depth;
    double casing_thickness;
    double threshold_depth;
    double threshold_thickness;
    double threshold_offset;
};

struct DoorPanelProperties {
    double panel_depth;
    double panel_width;
    double frame_depth;
    double frame_thickness;
};

DoorLiningProperties parse_lining(const std::vector<double>& values) {
    if (values.size() != 12) {
        throw std::runtime_error("Expected 12 door lining properties");
    }
    return {values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8], values[9], values[10], values[11]};
}

DoorPanelProperties parse_panel(const std::vector<double>& values) {
    if (values.size() != 4) {
        throw std::runtime_error("Expected 4 door panel properties");
    }
    return {values[0], values[1], values[2], values[3]};
}

express::Base create_door_lining(
    ifcopenshell::file* file,
    const std::vector<double>& size,
    const std::vector<double>& thickness,
    const std::vector<double>& position)
{
    const double th_side = thickness[0];
    const double th_up = thickness[1];
    auto lining_curve = ifcapi::detail::polyline(
        file,
        {
            ifcapi::detail::v2(0.0, 0.0),
            ifcapi::detail::v2(0.0, size[2]),
            ifcapi::detail::v2(size[0], size[2]),
            ifcapi::detail::v2(size[0], 0.0),
            ifcapi::detail::v2(size[0] - th_side, 0.0),
            ifcapi::detail::v2(size[0] - th_side, size[2] - th_up),
            ifcapi::detail::v2(th_side, size[2] - th_up),
            ifcapi::detail::v2(th_side, 0.0),
        },
        true);
    return ifcapi::detail::extrude_y(file, lining_curve, size[1], position);
}

express::Base create_box(ifcopenshell::file* file, const std::vector<double>& size, const std::vector<double>& position) {
    auto rect = ifcapi::detail::rectangle(file, {size[0], size[1]});
    return ifcapi::detail::extrude_z(file, rect, size[2], position);
}

std::vector<express::Base> create_door_sliding_panel_2d(
    ifcopenshell::file* file,
    const std::vector<double>& panel_size,
    const std::vector<double>& panel_position,
    const std::string& swing)
{
    auto door = ifcapi::detail::rectangle(file, panel_size, {panel_position[0] - panel_size[0] * 0.5, panel_position[1]}, true);
    if (swing == "RIGHT") {
        ifcapi::bindings::shape_builder_mirror(
            file,
            ifcapi::bindings::ShapeBuilderMirrorOptions{
                door, {1.0, 0.0}, {panel_position[0] + panel_size[0] / 2.0, panel_position[1]}, false, {}});
    }
    return {door};
}

std::vector<express::Base> create_door_panel_2d(
    ifcopenshell::file* file,
    std::vector<double> panel_size,
    const std::vector<double>& panel_position,
    const std::string& swing,
    bool sliding,
    bool double_swing)
{
    if (sliding) {
        return create_door_sliding_panel_2d(file, panel_size, panel_position, swing);
    }

    std::vector<express::Base> items;
    const std::vector<double> size_yx = {panel_size[1], panel_size[0]};
    if (double_swing) {
        items.push_back(ifcapi::detail::polyline(
            file,
            {ifcapi::detail::v2(0.0, 0.0), ifcapi::detail::v2(0.0, -size_yx[1]), ifcapi::detail::v2(size_yx[0], -size_yx[1])},
            false));
    }
    const std::vector<int> trim_mask = double_swing ? std::vector<int>{3, 1} : std::vector<int>{0, 1};
    items.push_back(ifcapi::bindings::shape_builder_ellipse_curve(
        file,
        ifcapi::bindings::ShapeBuilderEllipseCurveOptions{
            size_yx[1] - size_yx[0], size_yx[1], {size_yx[0], 0.0}, {}, {}, trim_mask}));
    items.push_back(ifcapi::detail::rectangle(file, size_yx));
    ifcapi::detail::translate_items(file, items, panel_position);
    if (swing == "RIGHT") {
        const auto mirror_point = ifcapi::detail::v2(panel_position[0] + size_yx[1] / 2.0, panel_position[1]);
        for (auto item : items) {
            ifcapi::bindings::shape_builder_mirror(
                file,
                ifcapi::bindings::ShapeBuilderMirrorOptions{item, {1.0, 0.0}, mirror_point, false, {}});
        }
    }
    return items;
}

std::vector<express::Base> create_door_panel_3d(
    ifcopenshell::file* file,
    const std::vector<double>& panel_size,
    const std::vector<double>& panel_position,
    const std::string& swing,
    const std::vector<double>& handle_size,
    const std::vector<double>& handle_offset)
{
    std::vector<express::Base> items;
    items.push_back(create_box(file, panel_size, panel_position));
    const std::vector<double> handle_center_offset = {handle_size[1] / 4.0, 0.0, handle_size[2] / 2.0};
    const std::vector<double> handle_position = {
        panel_position[0] + handle_offset[0] - handle_center_offset[0],
        panel_position[1] + handle_offset[1] - handle_center_offset[1],
        panel_position[2] + handle_offset[2] - handle_center_offset[2],
    };
    auto handle_polyline = ifcapi::detail::polyline(
        file,
        {
            ifcapi::detail::v2(0.0, 0.0),
            ifcapi::detail::v2(0.0, -handle_size[1]),
            ifcapi::detail::v2(handle_size[0], -handle_size[1]),
            ifcapi::detail::v2(handle_size[0], -handle_size[1] / 2.0),
            ifcapi::detail::v2(handle_size[1] / 2.0, -handle_size[1] / 2.0),
            ifcapi::detail::v2(handle_size[1] / 2.0, 0.0),
        },
        true);
    auto door_handle = ifcapi::bindings::shape_builder_extrude(
        file,
        ifcapi::bindings::ShapeBuilderExtrudeOptions{
            handle_polyline,
            handle_size[2],
            handle_position,
            {0.0, 0.0, 1.0},
            {0.0, 0.0, 1.0},
            {1.0, 0.0, 0.0},
            {}});
    items.push_back(door_handle);
    if (swing == "LEFT") {
        ifcapi::bindings::shape_builder_mirror(
            file,
            ifcapi::bindings::ShapeBuilderMirrorOptions{
                door_handle, {1.0, 0.0}, {panel_position[0] + panel_size[0] / 2.0, panel_position[1]}, false, {}});
    }
    auto mirrored = ifcapi::bindings::shape_builder_mirror(
        file,
        ifcapi::bindings::ShapeBuilderMirrorOptions{
            door_handle, {0.0, 1.0}, {handle_position[0], handle_position[1] + panel_size[1] / 2.0}, true, {}});
    items.push_back(mirrored);
    return items;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base geometry_add_door_representation(
    ifcopenshell::file* file,
    const GeometryAddDoorRepresentationOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !options.context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        express::Base context_value = options.context;
        express::Base* context = &context_value;
        const double overall_height = options.overall_height;
        const double overall_width = options.overall_width;
        const std::string& operation_type = options.operation_type;
        const double unit_scale = options.unit_scale;
        const auto lining = parse_lining(options.lining_properties);
        const auto panel = parse_panel(options.panel_properties);
        express::Base* part_of_product = options.part_of_product ? const_cast<express::Base*>(&*options.part_of_product) : nullptr;
        const bool double_swing_door = operation_type.find("DOUBLE_SWING") != std::string::npos;
        const bool double_door = operation_type.find("DOUBLE_DOOR") != std::string::npos;
        const bool sliding_door = operation_type.find("SLIDING") != std::string::npos;
        const std::string target_view = ifcapi::detail::read_string_attr(ifcapi::detail::deref_or_empty(context), "TargetView");

        if (target_view == "ELEVATION_VIEW") {
            auto rect = ifcapi::detail::rectangle(file, {overall_width, 0.0, overall_height});
            return shape_builder_representation(
                file,
                ShapeBuilderRepresentationOptions{*context, {rect}, "Curve3D"});
        }

        const double lining_depth = lining.lining_depth;
        const double lining_thickness_default = lining.lining_thickness;
        const double lining_offset = lining.lining_offset;
        const double lining_to_panel_offset_x = sliding_door ? lining_thickness_default : lining.lining_to_panel_offset_x;
        const double panel_depth = panel.panel_depth;
        const double lining_to_panel_offset_y_full = sliding_door ? -panel_depth : lining.lining_to_panel_offset_y;
        double transom_thickness = lining.transom_thickness / 2.0;
        double transom_offset = lining.transom_offset;
        if (transom_thickness == 0.0) {
            transom_offset = 0.0;
        }
        const double window_lining_height = overall_height - transom_offset - transom_thickness;
        double side_lining_thickness = lining_thickness_default;
        const double panel_lining_overlap_x = sliding_door ? 0.0 : std::max(lining_thickness_default - lining_to_panel_offset_x, 0.0);
        double top_lining_thickness = transom_thickness != 0.0 ? transom_thickness : lining_thickness_default;
        const double panel_top_lining_overlap_x = sliding_door ? 0.0 : std::max(top_lining_thickness - lining_to_panel_offset_x, 0.0);
        const double door_opening_width = overall_width - lining_to_panel_offset_x * 2.0;
        if (double_swing_door) {
            side_lining_thickness -= panel_lining_overlap_x;
            top_lining_thickness -= panel_top_lining_overlap_x;
        }
        const double threshold_width = overall_width - side_lining_thickness * 2.0;
        const double panel_width = door_opening_width * panel.panel_width;
        const double frame_height = window_lining_height - lining_to_panel_offset_x * 2.0;
        const double glass_thickness = to_unit(0.01, unit_scale);
        const std::vector<double> handle_size = {to_unit(mm(120), unit_scale), to_unit(mm(40), unit_scale), to_unit(mm(20), unit_scale)};
        const std::vector<double> handle_offset = {to_unit(mm(60), unit_scale), 0.0, to_unit(mm(1000), unit_scale)};
        const double slider_arrow_symbol_size = to_unit(mm(30), unit_scale);
        double panel_height = 0.0;
        double lining_height = 0.0;
        if (transom_offset != 0.0) {
            panel_height = transom_offset + transom_thickness - lining_to_panel_offset_x - lining.threshold_thickness;
            lining_height = transom_offset + transom_thickness;
        } else {
            panel_height = overall_height - lining_to_panel_offset_x - lining.threshold_thickness;
            lining_height = overall_height;
        }

        const std::vector<double> lining_size = {overall_width, lining_depth, lining_height};
        const std::vector<double> lining_thickness = {side_lining_thickness, top_lining_thickness};
        auto l_shape_check = [&](const std::vector<double>& thickness) {
            return lining_to_panel_offset_y_full < lining_depth &&
                std::any_of(thickness.begin(), thickness.end(), [&](double th) { return lining_to_panel_offset_x < th; });
        };

        if (target_view == "PLAN_VIEW") {
            std::vector<express::Base> items_2d;
            std::vector<double> panel_size = {panel_width, panel_depth};
            std::vector<double> panel_position = sliding_door
                ? ifcapi::detail::v2(lining_to_panel_offset_x, -panel_size[1])
                : ifcapi::detail::v2(lining_to_panel_offset_x, lining_depth);

            if (ifcapi::detail::read_string_attr(ifcapi::detail::deref_or_empty(context), "ContextIdentifier") == "Annotation") {
                if (!sliding_door) {
                    return {};
                }
                std::vector<express::Base> arrow_symbol;
                const double arrow_offset = slider_arrow_symbol_size / std::cos(15.0 * PI / 180.0);
                arrow_symbol.push_back(ifcapi::detail::polyline(
                    file, {ifcapi::detail::v2(0.35 * panel_size[0], 0.0), ifcapi::detail::v2(0.65 * panel_size[0], 0.0)}, false));
                arrow_symbol.push_back(ifcapi::bindings::shape_builder_polyline(
                    file,
                    ifcapi::bindings::ShapeBuilderPolylineOptions{
                        {
                            ifcapi::detail::v2(slider_arrow_symbol_size, arrow_offset),
                            ifcapi::detail::v2(0.0, 0.0),
                            ifcapi::detail::v2(slider_arrow_symbol_size, -arrow_offset),
                        },
                        false,
                        std::vector<double>{0.35 * panel_size[0], 0.0},
                        {}}));
                ifcapi::detail::translate_items(file, arrow_symbol, {panel_position[0], panel_position[1] - arrow_offset * 1.5});
                return shape_builder_representation(
                    file,
                    ShapeBuilderRepresentationOptions{*context, ifcapi::detail::const_refs(arrow_symbol), "Curve2D"});
            }

            express::Base lining_item = {};
            if (l_shape_check({side_lining_thickness})) {
                lining_item = ifcapi::detail::polyline(
                    file,
                    {
                        ifcapi::detail::v2(0.0, 0.0),
                        ifcapi::detail::v2(0.0, lining_depth),
                        ifcapi::detail::v2(lining_to_panel_offset_x, lining_depth),
                        ifcapi::detail::v2(lining_to_panel_offset_x, lining_to_panel_offset_y_full),
                        ifcapi::detail::v2(lining_thickness_default, lining_to_panel_offset_y_full),
                        ifcapi::detail::v2(lining_thickness_default, 0.0),
                    },
                    true);
            } else {
                lining_item = ifcapi::detail::rectangle(file, {side_lining_thickness, lining_depth});
            }
            items_2d.push_back(lining_item);
            items_2d.push_back(shape_builder_mirror(
                file,
                ShapeBuilderMirrorOptions{lining_item, {1.0, 0.0}, {overall_width / 2.0, 0.0}, true, {}}));
            std::vector<express::Base> door_items;
            if (double_door) {
                panel_size[0] /= 2.0;
                ifcapi::detail::append_items(door_items, create_door_panel_2d(file, panel_size, panel_position, "LEFT", sliding_door, double_swing_door));
                const auto mirror_point = ifcapi::detail::v2(panel_position[0] + door_opening_width / 2.0, panel_position[1]);
                std::vector<express::Base> mirrored;
                for (auto item : door_items) {
                    mirrored.push_back(shape_builder_mirror(
                        file,
                        ShapeBuilderMirrorOptions{item, {1.0, 0.0}, mirror_point, true, {}}));
                }
                ifcapi::detail::append_items(door_items, mirrored);
            } else {
                const std::string swing = operation_type.size() >= 4 && operation_type.substr(operation_type.size() - 4) == "LEFT" ? "LEFT" : "RIGHT";
                ifcapi::detail::append_items(door_items, create_door_panel_2d(file, panel_size, panel_position, swing, sliding_door, double_swing_door));
            }
            ifcapi::detail::append_items(items_2d, door_items);
            ifcapi::detail::translate_items(file, items_2d, {0.0, lining_offset});
            return shape_builder_representation(
                file,
                ShapeBuilderRepresentationOptions{*context, ifcapi::detail::const_refs(items_2d), "Curve2D"});
        }

        std::vector<express::Base> lining_items;
        std::vector<double> main_lining_size = lining_size;
        if (l_shape_check(lining_thickness)) {
            main_lining_size[1] = lining_to_panel_offset_y_full;
            std::vector<double> second_lining_size = lining_size;
            second_lining_size[1] = lining_size[1] - lining_to_panel_offset_y_full;
            std::vector<double> second_lining_thickness = {
                std::min(lining_thickness[0], lining_to_panel_offset_x),
                std::min(lining_thickness[1], lining_to_panel_offset_x),
            };
            lining_items.push_back(create_door_lining(file, second_lining_size, second_lining_thickness, {0.0, lining_to_panel_offset_y_full, 0.0}));
        }
        lining_items.push_back(create_door_lining(file, main_lining_size, lining_thickness, {0.0, 0.0, 0.0}));

        std::vector<express::Base> threshold_items;
        if (lining.threshold_thickness != 0.0) {
            threshold_items.push_back(create_box(
                file,
                {threshold_width, lining.threshold_depth, lining.threshold_thickness},
                {side_lining_thickness, lining.threshold_offset, 0.0}));
        }

        std::vector<express::Base> casing_items;
        if (lining_offset == 0.0 && lining.casing_thickness != 0.0) {
            const double casing_wall_overlap = std::max(lining.casing_thickness - lining_thickness_default, 0.0);
            const std::vector<double> inner_casing_thickness = {
                lining.casing_thickness - panel_lining_overlap_x,
                lining.casing_thickness - panel_top_lining_overlap_x,
            };
            const std::vector<double> casing_size = {overall_width + casing_wall_overlap * 2.0, lining.casing_depth, overall_height + casing_wall_overlap};
            casing_items.push_back(create_door_lining(
                file,
                casing_size,
                double_swing_door ? inner_casing_thickness : std::vector<double>{lining.casing_thickness, lining.casing_thickness},
                {-casing_wall_overlap, -lining.casing_depth, 0.0}));
            casing_items.push_back(create_door_lining(
                file,
                casing_size,
                inner_casing_thickness,
                {-casing_wall_overlap, lining_depth, 0.0}));
        }

        std::vector<express::Base> door_items;
        std::vector<double> panel_size = {panel_width, panel_depth, panel_height};
        const std::vector<double> panel_position = {lining_to_panel_offset_x, lining_to_panel_offset_y_full, lining.threshold_thickness};
        if (double_door) {
            const double double_door_offset = to_unit(0.001, unit_scale);
            panel_size[0] = panel_size[0] / 2.0 - double_door_offset;
            ifcapi::detail::append_items(door_items, create_door_panel_3d(file, panel_size, panel_position, "LEFT", handle_size, handle_offset));
            const auto mirror_point = ifcapi::detail::v2(panel_position[0] + door_opening_width / 2.0, panel_position[1]);
            std::vector<express::Base> mirrored;
            for (auto item : door_items) {
                mirrored.push_back(shape_builder_mirror(
                    file,
                    ShapeBuilderMirrorOptions{item, {1.0, 0.0}, mirror_point, true, {}}));
            }
            ifcapi::detail::append_items(door_items, mirrored);
        } else {
            const std::string swing = operation_type.size() >= 4 && operation_type.substr(operation_type.size() - 4) == "LEFT" ? "LEFT" : "RIGHT";
            ifcapi::detail::append_items(door_items, create_door_panel_3d(file, panel_size, panel_position, swing, handle_size, handle_offset));
        }

        std::vector<express::Base> window_lining_items;
        std::vector<express::Base> frame_items;
        std::vector<express::Base> glass_items;
        if (transom_thickness != 0.0) {
            auto current_window_items = ifcapi::detail::create_ifc_window(
                file,
                {overall_width, lining_depth, window_lining_height},
                {side_lining_thickness, lining_thickness_default, side_lining_thickness, transom_thickness},
                lining_to_panel_offset_x,
                lining_to_panel_offset_y_full,
                {door_opening_width, panel.frame_depth, frame_height},
                panel.frame_thickness,
                glass_thickness,
                {0.0, 0.0, overall_height - window_lining_height});
            window_lining_items = current_window_items.lining;
            frame_items = current_window_items.framing;
            glass_items = current_window_items.glazing;
        }

        std::vector<express::Base> lining_offset_items;
        ifcapi::detail::append_items(lining_offset_items, lining_items);
        ifcapi::detail::append_items(lining_offset_items, door_items);
        ifcapi::detail::append_items(lining_offset_items, window_lining_items);
        ifcapi::detail::append_items(lining_offset_items, frame_items);
        ifcapi::detail::append_items(lining_offset_items, glass_items);
        ifcapi::detail::translate_items(file, lining_offset_items, {0.0, lining_offset, 0.0});

        std::vector<express::Base> output_items = lining_offset_items;
        ifcapi::detail::append_items(output_items, threshold_items);
        ifcapi::detail::append_items(output_items, casing_items);
        auto representation = shape_builder_representation(
            file,
            ShapeBuilderRepresentationOptions{*context, ifcapi::detail::const_refs(output_items), {}});
        if (part_of_product) {
            std::vector<express::Base> lining_aspect = lining_items;
            ifcapi::detail::append_items(lining_aspect, window_lining_items);
            ifcapi::detail::append_items(lining_aspect, threshold_items);
            ifcapi::detail::append_items(lining_aspect, casing_items);
            geometry_add_shape_aspect(
                file,
                GeometryAddShapeAspectOptions{
                    "Lining", ifcapi::detail::const_refs(lining_aspect), representation, *part_of_product, {}});
            std::vector<express::Base> framing_aspect = door_items;
            ifcapi::detail::append_items(framing_aspect, frame_items);
            geometry_add_shape_aspect(
                file,
                GeometryAddShapeAspectOptions{
                    "Framing", ifcapi::detail::const_refs(framing_aspect), representation, *part_of_product, {}});
            if (!glass_items.empty()) {
                geometry_add_shape_aspect(
                    file,
                    GeometryAddShapeAspectOptions{
                        "Glazing", ifcapi::detail::const_refs(glass_items), representation, *part_of_product, {}});
            }
        }
        return representation;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
