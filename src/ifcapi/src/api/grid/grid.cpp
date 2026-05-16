// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/grid.h"
#include "ifcapi/detail/attribute.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcSchema.h"

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* grid_create_grid_axis(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* grid,
    const std::string& axis_tag,
    bool same_sense,
    const std::string& uvw_axes)
{
    ifcopenshell_clear_error();
    try {
        if (!file || !grid) return nullptr;
        int axes_idx = ifcapi::detail::attr_index_of(grid, uvw_axes.c_str());
        if (axes_idx < 0) {
            set_error("Invalid grid axis aggregate");
            return nullptr;
        }
        auto* axis = file->create(file->schema()->declaration_by_name("IfcGridAxis"));
        ifcapi::detail::write_string_attr(axis, "AxisTag", axis_tag);
        if (int same_sense_idx = ifcapi::detail::attr_index_of(axis, "SameSense"); same_sense_idx >= 0) {
            axis->set_attribute_value(static_cast<size_t>(same_sense_idx), same_sense);
        }
        auto axes = ifcapi::detail::read_ref_aggregate(grid, uvw_axes.c_str());
        axes.push_back(axis);
        ifcapi::detail::set_ref_aggregate(grid, axes_idx, axes);
        return axis;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void grid_remove_grid_axis(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* axis) {
    ifcopenshell_clear_error();
    try {
        auto* axis_curve = ifcapi::detail::read_ref_attr(axis, "AxisCurve");
        file->removeEntity(axis);
        if (axis_curve) entity_remove_deep2(axis_curve);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
