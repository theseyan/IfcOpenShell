// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/bindings/unit.h"
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

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }

struct LiningProperties {
    double lining_depth;
    double lining_thickness;
    double lining_offset;
    double lining_to_panel_offset_x;
    double lining_to_panel_offset_y;
    double mullion_thickness;
    double first_mullion_offset;
    double second_mullion_offset;
    double transom_thickness;
    double first_transom_offset;
    double second_transom_offset;
};

struct PanelProperties {
    double frame_depth;
    double frame_thickness;
};

inline double mm(double value) { return value / 1000.0; }
inline double to_unit(double value, double unit_scale) { return value / unit_scale; }

void require_finite(double value, const char* name) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument(std::string("Window ") + name + " must be finite");
    }
}

LiningProperties resolve_lining_properties(
    const std::optional<ifcapi::bindings::GeometryWindowLiningProperties>& value,
    double unit_scale)
{
    const auto properties = value.value_or(ifcapi::bindings::GeometryWindowLiningProperties{});
    LiningProperties result{
        properties.lining_depth.value_or(to_unit(mm(50.0), unit_scale)),
        properties.lining_thickness.value_or(to_unit(mm(50.0), unit_scale)),
        properties.lining_offset.value_or(to_unit(mm(50.0), unit_scale)),
        properties.lining_to_panel_offset_x.value_or(to_unit(mm(25.0), unit_scale)),
        properties.lining_to_panel_offset_y.value_or(to_unit(mm(25.0), unit_scale)),
        properties.mullion_thickness.value_or(to_unit(mm(50.0), unit_scale)),
        properties.first_mullion_offset.value_or(to_unit(mm(300.0), unit_scale)),
        properties.second_mullion_offset.value_or(to_unit(mm(450.0), unit_scale)),
        properties.transom_thickness.value_or(to_unit(mm(50.0), unit_scale)),
        properties.first_transom_offset.value_or(to_unit(mm(300.0), unit_scale)),
        properties.second_transom_offset.value_or(to_unit(mm(600.0), unit_scale)),
    };
    const double values[] = {
        result.lining_depth, result.lining_thickness, result.lining_offset,
        result.lining_to_panel_offset_x, result.lining_to_panel_offset_y,
        result.mullion_thickness, result.first_mullion_offset,
        result.second_mullion_offset, result.transom_thickness,
        result.first_transom_offset, result.second_transom_offset};
    for (double item : values) require_finite(item, "lining property");
    return result;
}

std::vector<PanelProperties> resolve_panel_properties(
    const std::optional<std::vector<ifcapi::bindings::GeometryWindowPanelProperties>>& value,
    double unit_scale)
{
    const auto properties = value.value_or(
        std::vector<ifcapi::bindings::GeometryWindowPanelProperties>{
            ifcapi::bindings::GeometryWindowPanelProperties{}});
    std::vector<PanelProperties> result;
    result.reserve(properties.size());
    for (const auto& panel : properties) {
        PanelProperties resolved{
            panel.frame_depth.value_or(to_unit(mm(35.0), unit_scale)),
            panel.frame_thickness.value_or(to_unit(mm(35.0), unit_scale)),
        };
        require_finite(resolved.frame_depth, "panel property");
        require_finite(resolved.frame_thickness, "panel property");
        result.push_back(resolved);
    }
    return result;
}

std::vector<std::vector<int>> panel_schema(ifcapi::bindings::GeometryWindowPartitionType partition_type) {
    using Partition = ifcapi::bindings::GeometryWindowPartitionType;
    switch (partition_type) {
    case Partition::SINGLE_PANEL:
        return {{0}};
    case Partition::DOUBLE_PANEL_HORIZONTAL:
        return {{1}, {0}};
    case Partition::DOUBLE_PANEL_VERTICAL:
        return {{0, 1}};
    case Partition::TRIPLE_PANEL_BOTTOM:
        return {{2, 2}, {0, 1}};
    case Partition::TRIPLE_PANEL_HORIZONTAL:
        return {{2}, {1}, {0}};
    case Partition::TRIPLE_PANEL_LEFT:
        return {{0, 2}, {0, 1}};
    case Partition::TRIPLE_PANEL_RIGHT:
        return {{2, 1}, {0, 1}};
    case Partition::TRIPLE_PANEL_TOP:
        return {{1, 2}, {0, 0}};
    case Partition::TRIPLE_PANEL_VERTICAL:
        return {{0, 1, 2}};
    }
    throw std::invalid_argument("Unsupported window partition type");
}

express::Base create_window_2d_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<std::vector<int>>& panel_schema,
    const std::vector<PanelProperties>& panels,
    const LiningProperties& lining,
    double overall_width,
    double lining_depth,
    double lining_thickness,
    double lining_offset)
{
    std::vector<express::Base> items_2d;
    const auto& top_row = panel_schema.back();
    std::vector<int> unique_values = top_row;
    std::sort(unique_values.begin(), unique_values.end());
    unique_values.erase(std::unique(unique_values.begin(), unique_values.end()), unique_values.end());
    const size_t unique_cols = unique_values.size();
    std::vector<int> built_panels;
    double accumulated_width = 0.0;
    const double mullion_thickness = lining.mullion_thickness / 2.0;

    for (size_t column_i = 0; column_i < top_row.size(); ++column_i) {
        const int panel_i = top_row[column_i];
        if (std::find(built_panels.begin(), built_panels.end(), panel_i) != built_panels.end()) {
            continue;
        }

        std::vector<express::Base> cur_panel_items;
        std::vector<double> window_lining_thickness = {lining_thickness, lining_thickness};
        std::vector<bool> closed_lining = {true, true};
        const bool has_mullion = unique_cols > 1;
        const bool first_column = column_i == 0;
        const bool last_column = column_i == unique_cols - 1;
        const bool left_to_mullion = has_mullion && !last_column;
        const bool right_to_mullion = has_mullion && !first_column;

        double panel_width = overall_width;
        if (has_mullion) {
            if (first_column) {
                panel_width = lining.first_mullion_offset;
            } else if (last_column) {
                panel_width = overall_width - accumulated_width;
            } else {
                panel_width = lining.second_mullion_offset - accumulated_width;
            }
            if (!first_column) {
                window_lining_thickness[0] = mullion_thickness;
                closed_lining[0] = false;
            }
            if (!last_column) {
                window_lining_thickness[1] = mullion_thickness;
                closed_lining[1] = false;
            }
        }

        const auto& panel = panels.at(static_cast<size_t>(panel_i));
        const double lining_to_panel_offset_y_full = (lining_depth - panel.frame_depth) + lining.lining_to_panel_offset_y;
        const double base_frame_clear = lining.lining_to_panel_offset_x + panel.frame_thickness - lining_thickness;
        const double current_offset_x = base_frame_clear - panel.frame_thickness + mullion_thickness;

        cur_panel_items.push_back(ifcapi::detail::polyline(
            file,
            {ifcapi::detail::v2(window_lining_thickness[0], 0.0), ifcapi::detail::v2(panel_width - window_lining_thickness[1], 0.0)},
            false));

        auto lining_shape = [&](double thickness, bool closed, bool mirror, double x_offset) {
            express::Base shape = {};
            if (ifcapi::detail::window_l_shape_check(lining_to_panel_offset_y_full, lining_depth, {x_offset}, {thickness})) {
                shape = ifcapi::detail::polyline(
                    file,
                    {
                        ifcapi::detail::v2(0.0, lining_depth),
                        ifcapi::detail::v2(x_offset, lining_depth),
                        ifcapi::detail::v2(x_offset, lining_to_panel_offset_y_full),
                        ifcapi::detail::v2(thickness, lining_to_panel_offset_y_full),
                        ifcapi::detail::v2(thickness, 0.0),
                        ifcapi::detail::v2(0.0, 0.0),
                    },
                    closed);
            } else {
                shape = ifcapi::detail::polyline(
                    file,
                    {
                        ifcapi::detail::v2(0.0, lining_depth),
                        ifcapi::detail::v2(thickness, lining_depth),
                        ifcapi::detail::v2(thickness, 0.0),
                        ifcapi::detail::v2(0.0, 0.0),
                    },
                    closed);
            }
            if (mirror) {
                ifcapi::bindings::shape_builder_mirror(
                    file,
                    ifcapi::bindings::ShapeBuilderMirrorOptions{
                        shape,
                        ifcapi::bindings::Vec2{1.0, 0.0},
                        ifcapi::bindings::Vec2{panel_width / 2.0, 0.0},
                        false,
                        {}});
            }
            return shape;
        };

        cur_panel_items.push_back(lining_shape(
            window_lining_thickness[0], closed_lining[0], false, right_to_mullion ? current_offset_x : lining.lining_to_panel_offset_x));
        cur_panel_items.push_back(lining_shape(
            window_lining_thickness[1], closed_lining[1], true, left_to_mullion ? current_offset_x : lining.lining_to_panel_offset_x));

        std::vector<express::Base> frame_items;
        const std::vector<double> frame_position = {
            right_to_mullion ? current_offset_x : lining.lining_to_panel_offset_x,
            lining_to_panel_offset_y_full,
        };
        double frame_width = panel_width;
        frame_width -= left_to_mullion ? current_offset_x : lining.lining_to_panel_offset_x;
        frame_width -= right_to_mullion ? current_offset_x : lining.lining_to_panel_offset_x;

        auto frame_vertical = ifcapi::detail::rectangle(file, {panel.frame_thickness, panel.frame_depth});
        frame_items.push_back(frame_vertical);
        frame_items.push_back(ifcapi::bindings::shape_builder_mirror(
            file,
            ifcapi::bindings::ShapeBuilderMirrorOptions{
                frame_vertical,
                ifcapi::bindings::Vec2{1.0, 0.0},
                ifcapi::bindings::Vec2{frame_width / 2.0, 0.0},
                true,
                {}}));
        auto frame_horizontal = ifcapi::detail::polyline(
            file,
            {ifcapi::detail::v2(panel.frame_thickness, 0.0), ifcapi::detail::v2(frame_width - panel.frame_thickness, 0.0)},
            false);
        frame_items.push_back(frame_horizontal);
        frame_items.push_back(ifcapi::bindings::shape_builder_translate(
            file,
            ifcapi::bindings::ShapeBuilderTranslateOptions{
                frame_horizontal, ifcapi::bindings::Vec2{0.0, panel.frame_depth}, true}));
        frame_items.push_back(ifcapi::bindings::shape_builder_translate(
            file,
            ifcapi::bindings::ShapeBuilderTranslateOptions{
                frame_horizontal, ifcapi::bindings::Vec2{0.0, panel.frame_depth / 2.0}, true}));
        ifcapi::detail::translate_items(file, frame_items, frame_position);
        ifcapi::detail::append_items(cur_panel_items, frame_items);

        ifcapi::detail::translate_items(file, cur_panel_items, {accumulated_width, 0.0});
        accumulated_width += panel_width;
        built_panels.push_back(panel_i);
        ifcapi::detail::append_items(items_2d, cur_panel_items);
    }

    ifcapi::detail::translate_items(file, items_2d, {0.0, lining_offset});
    return ifcapi::bindings::shape_builder_representation(
        file,
        ifcapi::bindings::ShapeBuilderRepresentationOptions{*context, ifcapi::detail::const_refs(items_2d), {}});
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base geometry_add_window_representation(
    ifcopenshell::file* file,
    const GeometryAddWindowRepresentationOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !options.context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        express::Base context_value = options.context;
        express::Base* context = &context_value;
        if (options.context.file() != file) {
            throw std::invalid_argument("Window context must belong to the supplied file");
        }
        if (options.part_of_product && options.part_of_product->file() != file) {
            throw std::invalid_argument("Window shape aspect product must belong to the supplied file");
        }
        const double unit_scale = options.unit_scale.value_or(unit_calculate_unit_scale(file, "LENGTHUNIT"));
        require_finite(unit_scale, "unit scale");
        if (unit_scale <= 0.0) throw std::invalid_argument("Window unit scale must be positive");
        const double overall_height = options.overall_height.value_or(to_unit(0.9, unit_scale));
        const double overall_width = options.overall_width.value_or(to_unit(0.6, unit_scale));
        require_finite(overall_height, "overall height");
        require_finite(overall_width, "overall width");
        if (overall_height <= 0.0 || overall_width <= 0.0) {
            throw std::invalid_argument("Window overall dimensions must be positive");
        }
        const double glass_thickness = to_unit(0.01, unit_scale);
        const auto lining = resolve_lining_properties(options.lining_properties, unit_scale);
        const auto panels = resolve_panel_properties(options.panel_properties, unit_scale);
        auto panel_schema = ::panel_schema(
            options.partition_type.value_or(GeometryWindowPartitionType::SINGLE_PANEL));
        express::Base* part_of_product = options.part_of_product ? const_cast<express::Base*>(&*options.part_of_product) : nullptr;
        const int max_panel_index = *std::max_element(
            panel_schema.front().begin(), panel_schema.front().end());
        int required_panel_index = max_panel_index;
        for (const auto& row : panel_schema) {
            required_panel_index = std::max(
                required_panel_index, *std::max_element(row.begin(), row.end()));
        }
        if (panels.size() <= static_cast<size_t>(required_panel_index)) {
            throw std::invalid_argument("Window panel properties do not cover the partition layout");
        }

        const std::string target_view = ifcapi::detail::read_string_attr(ifcapi::detail::deref_or_empty(context), "TargetView");
        if (target_view == "ELEVATION_VIEW") {
            auto rect = ifcapi::detail::rectangle(file, {overall_width, 0.0, overall_height});
            return shape_builder_representation(
                file,
                ShapeBuilderRepresentationOptions{*context, {rect}, {}});
        }

        const double lining_thickness = lining.lining_thickness;
        const double lining_depth = lining.lining_depth;
        const double lining_offset = lining.lining_offset;
        if (target_view == "PLAN_VIEW") {
            return create_window_2d_representation(
                file, context, panel_schema, panels, lining, overall_width, lining_depth, lining_thickness, lining_offset);
        }

        const double lining_to_panel_offset_x = lining.lining_to_panel_offset_x;
        const double lining_to_panel_offset_y = lining.lining_to_panel_offset_y;
        const double mullion_thickness = lining.mullion_thickness / 2.0;
        const double first_mullion_offset = lining.first_mullion_offset;
        const double second_mullion_offset = lining.second_mullion_offset;
        const double transom_thickness = lining.transom_thickness / 2.0;
        const double first_transom_offset = lining.first_transom_offset;
        const double second_transom_offset = lining.second_transom_offset;

        std::vector<size_t> unique_rows_in_col;
        for (size_t column_i = 0; column_i < panel_schema[0].size(); ++column_i) {
            std::vector<int> values;
            for (const auto& row : panel_schema) {
                values.push_back(row.at(column_i));
            }
            std::sort(values.begin(), values.end());
            values.erase(std::unique(values.begin(), values.end()), values.end());
            unique_rows_in_col.push_back(values.size());
        }

        std::vector<double> accumulated_height(panel_schema[0].size(), 0.0);
        std::vector<int> built_panels;
        std::vector<express::Base> window_items;
        std::vector<express::Base> lining_items;
        std::vector<express::Base> framing_items;
        std::vector<express::Base> glazing_items;

        for (size_t row_i = 0; row_i < panel_schema.size(); ++row_i) {
            const auto& panel_row = panel_schema[row_i];
            std::vector<int> unique_values = panel_row;
            std::sort(unique_values.begin(), unique_values.end());
            unique_values.erase(std::unique(unique_values.begin(), unique_values.end()), unique_values.end());
            const size_t unique_cols = unique_values.size();
            double accumulated_width = 0.0;

            for (size_t column_i = 0; column_i < panel_row.size(); ++column_i) {
                const int panel_i = panel_row[column_i];
                const bool has_mullion = unique_cols > 1;
                const bool first_column = column_i == 0;
                const bool last_column = column_i == unique_cols - 1;
                const bool left_to_mullion = has_mullion && !last_column;
                const bool right_to_mullion = has_mullion && !first_column;
                const bool has_transom = unique_rows_in_col[column_i] > 1;
                const bool first_row = row_i == 0;
                const bool last_row = row_i == unique_rows_in_col[column_i] - 1;
                const bool top_to_transom = has_transom && !first_row;
                const bool bottom_to_transom = has_transom && !last_row;

                double panel_width = overall_width;
                if (has_mullion) {
                    if (first_column) {
                        panel_width = first_mullion_offset;
                    } else if (last_column) {
                        panel_width = overall_width - accumulated_width;
                    } else {
                        panel_width = second_mullion_offset - accumulated_width;
                    }
                }

                double panel_height = overall_height;
                if (has_transom) {
                    if (first_row) {
                        panel_height = first_transom_offset;
                    } else if (last_row) {
                        panel_height = overall_height - accumulated_height[column_i];
                    } else {
                        panel_height = second_transom_offset - accumulated_height[column_i];
                    }
                }

                if (std::find(built_panels.begin(), built_panels.end(), panel_i) != built_panels.end()) {
                    accumulated_height[column_i] += panel_height;
                    accumulated_width += panel_width;
                    continue;
                }

                const auto& panel = panels.at(static_cast<size_t>(panel_i));
                const double lining_to_panel_offset_y_full = (lining_depth - panel.frame_depth) + lining_to_panel_offset_y;
                const double base_frame_clear = lining_to_panel_offset_x + panel.frame_thickness - lining_thickness;
                const double current_offset_x = base_frame_clear - panel.frame_thickness + mullion_thickness;
                const double current_offset_z = base_frame_clear - panel.frame_thickness + transom_thickness;
                const std::vector<double> window_lining_thickness = {
                    right_to_mullion ? mullion_thickness : lining_thickness,
                    bottom_to_transom ? transom_thickness : lining_thickness,
                    left_to_mullion ? mullion_thickness : lining_thickness,
                    top_to_transom ? transom_thickness : lining_thickness,
                };
                const std::vector<double> x_offsets = {
                    right_to_mullion ? current_offset_x : lining_to_panel_offset_x,
                    bottom_to_transom ? current_offset_z : lining_to_panel_offset_x,
                    left_to_mullion ? current_offset_x : lining_to_panel_offset_x,
                    top_to_transom ? current_offset_z : lining_to_panel_offset_x,
                };

                std::vector<double> window_lining_size = {panel_width, lining_depth, panel_height};
                std::vector<double> frame_size = window_lining_size;
                frame_size[1] = panel.frame_depth;
                frame_size[0] -= x_offsets[0] + x_offsets[2];
                frame_size[2] -= x_offsets[1] + x_offsets[3];

                auto current_items = ifcapi::detail::create_ifc_window(
                    file,
                    window_lining_size,
                    window_lining_thickness,
                    lining_to_panel_offset_x,
                    lining_to_panel_offset_y_full,
                    frame_size,
                    panel.frame_thickness,
                    glass_thickness,
                    {accumulated_width, 0.0, accumulated_height[column_i]},
                    x_offsets);
                built_panels.push_back(panel_i);
                ifcapi::detail::append_items(window_items, current_items.lining);
                ifcapi::detail::append_items(window_items, current_items.framing);
                ifcapi::detail::append_items(window_items, current_items.glazing);
                ifcapi::detail::append_items(lining_items, current_items.lining);
                ifcapi::detail::append_items(framing_items, current_items.framing);
                ifcapi::detail::append_items(glazing_items, current_items.glazing);

                accumulated_height[column_i] += panel_height;
                accumulated_width += panel_width;
            }
        }

        ifcapi::detail::translate_items(file, window_items, {0.0, lining_offset, 0.0});
        auto representation = shape_builder_representation(
            file,
            ShapeBuilderRepresentationOptions{*context, ifcapi::detail::const_refs(window_items), {}});
        if (part_of_product) {
            geometry_add_shape_aspect(
                file,
                GeometryAddShapeAspectOptions{
                    "Lining", ifcapi::detail::const_refs(lining_items), representation, *part_of_product, {}});
            geometry_add_shape_aspect(
                file,
                GeometryAddShapeAspectOptions{
                    "Framing", ifcapi::detail::const_refs(framing_items), representation, *part_of_product, {}});
            geometry_add_shape_aspect(
                file,
                GeometryAddShapeAspectOptions{
                    "Glazing", ifcapi::detail::const_refs(glazing_items), representation, *part_of_product, {}});
        }
        return representation;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
