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

LiningProperties parse_lining_properties(const std::vector<double>& values) {
    if (values.size() != 11) {
        throw std::runtime_error("Expected 11 window lining properties");
    }
    return {values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8], values[9], values[10]};
}

std::vector<PanelProperties> parse_panel_properties(const std::vector<std::vector<double>>& values) {
    std::vector<PanelProperties> result;
    result.reserve(values.size());
    for (const auto& row : values) {
        if (row.size() != 2) {
            throw std::runtime_error("Expected 2 window panel properties");
        }
        result.push_back({row[0], row[1]});
    }
    return result;
}

std::vector<std::vector<int>> reversed_schema(const std::vector<std::vector<int>>& panel_schema) {
    return {panel_schema.rbegin(), panel_schema.rend()};
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
                ifcapi::bindings::shape_builder_mirror(file, &shape, {1.0, 0.0}, {panel_width / 2.0, 0.0}, false, {});
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
        frame_items.push_back(ifcapi::bindings::shape_builder_mirror(file, &frame_vertical, {1.0, 0.0}, {frame_width / 2.0, 0.0}, true, {}));
        auto frame_horizontal = ifcapi::detail::polyline(
            file,
            {ifcapi::detail::v2(panel.frame_thickness, 0.0), ifcapi::detail::v2(frame_width - panel.frame_thickness, 0.0)},
            false);
        frame_items.push_back(frame_horizontal);
        frame_items.push_back(ifcapi::bindings::shape_builder_translate(file, &frame_horizontal, {0.0, panel.frame_depth}, true));
        frame_items.push_back(ifcapi::bindings::shape_builder_translate(file, &frame_horizontal, {0.0, panel.frame_depth / 2.0}, true));
        ifcapi::detail::translate_items(file, frame_items, frame_position);
        ifcapi::detail::append_items(cur_panel_items, frame_items);

        ifcapi::detail::translate_items(file, cur_panel_items, {accumulated_width, 0.0});
        accumulated_width += panel_width;
        built_panels.push_back(panel_i);
        ifcapi::detail::append_items(items_2d, cur_panel_items);
    }

    ifcapi::detail::translate_items(file, items_2d, {0.0, lining_offset});
    return ifcapi::bindings::shape_builder_representation(file, context, ifcapi::detail::const_refs(items_2d), nullptr);
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base geometry_add_window_representation(
    ifcopenshell::file* file,
    express::Base* context,
    double overall_height,
    double overall_width,
    const std::vector<std::vector<int>>& raw_panel_schema,
    const std::vector<double>& lining_property_values,
    const std::vector<std::vector<double>>& panel_property_values,
    express::Base* part_of_product,
    double glass_thickness)
{
    ifcopenshell_clear_error();
    if (!file || !context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const auto lining = parse_lining_properties(lining_property_values);
        const auto panels = parse_panel_properties(panel_property_values);
        auto panel_schema = reversed_schema(raw_panel_schema);
        if (panel_schema.empty() || panel_schema[0].empty()) {
            throw std::runtime_error("Invalid panel schema");
        }

        const std::string target_view = ifcapi::detail::read_string_attr(ifcapi::detail::deref_or_empty(context), "TargetView");
        if (target_view == "ELEVATION_VIEW") {
            auto rect = ifcapi::detail::rectangle(file, {overall_width, 0.0, overall_height});
            return shape_builder_representation(file, context, {rect}, nullptr);
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
        auto representation = shape_builder_representation(file, context, ifcapi::detail::const_refs(window_items), nullptr);
        if (part_of_product) {
            geometry_add_shape_aspect(file, "Lining", ifcapi::detail::const_refs(lining_items), &representation, part_of_product, nullptr, false);
            geometry_add_shape_aspect(file, "Framing", ifcapi::detail::const_refs(framing_items), &representation, part_of_product, nullptr, false);
            geometry_add_shape_aspect(file, "Glazing", ifcapi::detail::const_refs(glazing_items), &representation, part_of_product, nullptr, false);
        }
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
