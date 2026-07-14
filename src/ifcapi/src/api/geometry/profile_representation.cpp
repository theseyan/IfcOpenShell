// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/geometry.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline void set_error(const char* message) { ifcopenshell::capi::set_last_error(message); }

std::vector<double> cardinal_point_location(
    const std::optional<std::string>& cardinal_point,
    double x,
    double y)
{
    if (!cardinal_point) return {0.0, 0.0, 0.0};
    if (*cardinal_point == "bottom left") return {-x / 2.0, y / 2.0, 0.0};
    if (*cardinal_point == "bottom centre") return {0.0, y / 2.0, 0.0};
    if (*cardinal_point == "bottom right") return {x / 2.0, y / 2.0, 0.0};
    if (*cardinal_point == "mid-depth left") return {-x / 2.0, 0.0, 0.0};
    if (*cardinal_point == "mid-depth centre") return {0.0, 0.0, 0.0};
    if (*cardinal_point == "mid-depth right") return {x / 2.0, 0.0, 0.0};
    if (*cardinal_point == "top left") return {-x / 2.0, -y / 2.0, 0.0};
    if (*cardinal_point == "top centre") return {0.0, -y / 2.0, 0.0};
    if (*cardinal_point == "top right") return {x / 2.0, -y / 2.0, 0.0};
    return {0.0, 0.0, 0.0};
}

} // namespace

namespace ifcapi::bindings {

express::Base geometry_add_profile_representation(
    ifcopenshell::file* file,
    const GeometryAddProfileRepresentationOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !options.context || !options.profile) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const double unit_scale = unit_calculate_unit_scale(file, "LENGTHUNIT");
        auto profile = options.profile;
        const auto extents = geometry_profile_extents(file, &profile);
        if (extents.size() != 2) {
            throw std::runtime_error("Unable to determine profile extents");
        }

        const auto location = cardinal_point_location(options.cardinal_point, extents[0], extents[1]);
        const std::vector<double> default_z_axis = {0.0, 0.0, 1.0};
        const std::vector<double> default_x_axis = {1.0, 0.0, 0.0};
        const auto& z_axis = options.placement_z_axis.value_or(default_z_axis);
        const auto& x_axis = options.placement_x_axis.value_or(default_x_axis);

        auto placement = ifcapi::detail::create_axis2_placement_3d(file, location, z_axis, x_axis);
        auto extrusion = file->create(file->schema()->declaration_by_name("IfcExtrudedAreaSolid"));
        ifcapi::detail::write_ref_attr(extrusion, "SweptArea", options.profile);
        ifcapi::detail::write_ref_attr(extrusion, "Position", placement);
        ifcapi::detail::write_ref_attr(
            extrusion,
            "ExtrudedDirection",
            ifcapi::detail::create_direction(file, {0.0, 0.0, 1.0}));
        ifcapi::detail::write_double_attr(extrusion, "Depth", options.depth / unit_scale);

        express::Base item = ifcapi::detail::apply_ordered_clippings(
            file,
            extrusion,
            options.clipping_kinds,
            options.clipping_locations,
            options.clipping_normals,
            options.clipping_entities,
            unit_scale);

        auto representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        ifcapi::detail::write_ref_attr(representation, "ContextOfItems", options.context);
        ifcapi::detail::copy_string_attr_preserving_null(
            representation, "RepresentationIdentifier", options.context, "ContextIdentifier");
        ifcapi::detail::write_string_attr(
            representation, "RepresentationType", options.clipping_kinds.empty() ? "SweptSolid" : "Clipping");
        ifcapi::detail::write_ref_aggregate(representation, "Items", {item});
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

} // namespace ifcapi::bindings
