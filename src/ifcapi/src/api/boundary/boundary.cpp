// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/boundary.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/geometry.h"
#include "ifcapi/detail/relationship.h"
#include "ifcopenshell_api_internal.hpp"

#include <stdexcept>
#include <string>
#include <vector>

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

const char* physical_or_virtual_name(ifcapi::bindings::BoundaryPhysicalOrVirtual value) {
    using Classification = ifcapi::bindings::BoundaryPhysicalOrVirtual;
    switch (value) {
    case Classification::PHYSICAL: return "PHYSICAL";
    case Classification::VIRTUAL: return "VIRTUAL";
    case Classification::NOTDEFINED: return "NOTDEFINED";
    }
    throw std::invalid_argument("Unsupported physical or virtual boundary classification");
}

const char* internal_or_external_name(ifcapi::bindings::BoundaryInternalOrExternal value) {
    using Classification = ifcapi::bindings::BoundaryInternalOrExternal;
    switch (value) {
    case Classification::INTERNAL: return "INTERNAL";
    case Classification::EXTERNAL: return "EXTERNAL";
    case Classification::EXTERNAL_EARTH: return "EXTERNAL_EARTH";
    case Classification::EXTERNAL_WATER: return "EXTERNAL_WATER";
    case Classification::EXTERNAL_FIRE: return "EXTERNAL_FIRE";
    case Classification::NOTDEFINED: return "NOTDEFINED";
    }
    throw std::invalid_argument("Unsupported internal or external boundary classification");
}

template <typename T, std::size_t N>
std::vector<T> dynamic_array(const std::array<T, N>& values) {
    return {values.begin(), values.end()};
}

template <typename T, std::size_t N>
std::vector<std::vector<T>> dynamic_arrays(const std::vector<std::array<T, N>>& values) {
    std::vector<std::vector<T>> result;
    result.reserve(values.size());
    for (const auto& value : values) result.push_back(dynamic_array(value));
    return result;
}
}

namespace ifcapi {
namespace bindings {

express::Base boundary_copy_boundary(ifcopenshell::file* file, express::Base* boundary) {
    ifcopenshell_clear_error();
    try {
        auto result = ifcapi::detail::shallow_copy(file, boundary ? *boundary : express::Base());
        if (!result) return {};
        if (auto geometry = ifcapi::detail::read_ref_attr(result, "ConnectionGeometry")) {
            ifcapi::detail::write_ref_attr(result, "ConnectionGeometry", shape_builder_deep_copy(file, &geometry));
        }
        return result;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

void boundary_assign_connection_geometry(
    ifcopenshell::file* file,
    express::Base* rel_space_boundary,
    const BoundaryAssignConnectionGeometryOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !rel_space_boundary || !*rel_space_boundary) {
        set_error("Invalid arguments");
        return;
    }
    try {
        const double unit_scale = options.unit_scale.value_or(unit_calculate_unit_scale(file, "LENGTHUNIT"));
        static const std::vector<std::vector<std::array<double, 2>>> empty_inner_boundaries;
        const auto& inner_boundaries =
            options.inner_boundaries ? *options.inner_boundaries : empty_inner_boundaries;
        auto outer_curve = ifcapi::detail::create_closed_polyline(file, dynamic_arrays(options.outer_boundary), unit_scale);
        std::vector<express::Base> inner_curves;
        inner_curves.reserve(inner_boundaries.size());
        for (const auto& boundary : inner_boundaries) {
            inner_curves.push_back(ifcapi::detail::create_closed_polyline(file, dynamic_arrays(boundary), unit_scale));
        }

        auto curve_bounded_plane = file->create(file->schema()->declaration_by_name("IfcCurveBoundedPlane"));
        auto placement = ifcapi::detail::create_axis2_placement_3d(
            file,
            ifcapi::detail::scale_point_coordinates(dynamic_array(options.location), unit_scale),
            dynamic_array(options.axis),
            dynamic_array(options.ref_direction));
        ifcapi::detail::write_ref_attr(
            curve_bounded_plane, "BasisSurface", ifcapi::detail::create_plane(file, placement));
        ifcapi::detail::write_ref_attr(curve_bounded_plane, "OuterBoundary", outer_curve);
        ifcapi::detail::write_ref_aggregate(curve_bounded_plane, "InnerBoundaries", inner_curves);

        auto connection_geometry = file->create(file->schema()->declaration_by_name("IfcConnectionSurfaceGeometry"));
        ifcapi::detail::write_ref_attr(connection_geometry, "SurfaceOnRelatingElement", curve_bounded_plane);
        ifcapi::detail::write_ref_attr(*rel_space_boundary, "ConnectionGeometry", connection_geometry);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void boundary_remove_boundary(ifcopenshell::file* file, express::Base* boundary) {
    ifcopenshell_clear_error();
    if (!boundary || !*boundary) {
        return;
    }
    try {
        auto geometry = ifcapi::detail::read_ref_attr(*boundary, "ConnectionGeometry");
        if (geometry) {
            ifcapi::detail::write_ref_attr(*boundary, "ConnectionGeometry", {});
            entity_remove_deep(&geometry);
        }
        ifcapi::detail::remove_with_history(file, *boundary);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void boundary_edit_attributes(
    express::Base* entity,
    const BoundaryEditAttributesOptions& options)
{
    ifcopenshell_clear_error();
    if (!entity || !*entity) {
        set_error("Invalid arguments");
        return;
    }
    try {
        const char* physical_or_virtual = physical_or_virtual_name(options.physical_or_virtual);
        const char* internal_or_external = internal_or_external_name(options.internal_or_external);
        ifcapi::detail::write_ref_attr(*entity, "RelatingSpace", options.relating_space);
        ifcapi::detail::write_ref_attr(*entity, "RelatedBuildingElement", options.related_building_element);
        if (ifcapi::detail::entity_has_attr(*entity, "ParentBoundary")) {
            ifcapi::detail::write_ref_attr(*entity, "ParentBoundary", options.parent_boundary.value_or(express::Base()));
        }
        if (ifcapi::detail::entity_has_attr(*entity, "CorrespondingBoundary")) {
            ifcapi::detail::write_ref_attr(
                *entity, "CorrespondingBoundary", options.corresponding_boundary.value_or(express::Base()));
        }
        ifcapi::detail::write_enum_attr(*entity, "PhysicalOrVirtualBoundary", physical_or_virtual);
        ifcapi::detail::write_enum_attr(*entity, "InternalOrExternalBoundary", internal_or_external);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

} // namespace bindings
} // namespace ifcapi
