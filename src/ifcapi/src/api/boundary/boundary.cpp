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

#include <string>
#include <vector>

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
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
        auto outer_curve = ifcapi::detail::create_closed_polyline(file, options.outer_boundary, options.unit_scale);
        std::vector<express::Base> inner_curves;
        inner_curves.reserve(options.inner_boundaries.size());
        for (const auto& boundary : options.inner_boundaries) {
            inner_curves.push_back(ifcapi::detail::create_closed_polyline(file, boundary, options.unit_scale));
        }

        auto curve_bounded_plane = file->create(file->schema()->declaration_by_name("IfcCurveBoundedPlane"));
        auto placement = ifcapi::detail::create_axis2_placement_3d(
            file,
            ifcapi::detail::scale_point_coordinates(options.location, options.unit_scale),
            options.axis,
            options.ref_direction);
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
        ifcapi::detail::write_ref_attr(*entity, "RelatingSpace", options.relating_space);
        ifcapi::detail::write_ref_attr(*entity, "RelatedBuildingElement", options.related_building_element);
        if (ifcapi::detail::entity_has_attr(*entity, "ParentBoundary")) {
            ifcapi::detail::write_ref_attr(*entity, "ParentBoundary", options.parent_boundary.value_or(express::Base()));
        }
        if (ifcapi::detail::entity_has_attr(*entity, "CorrespondingBoundary")) {
            ifcapi::detail::write_ref_attr(
                *entity, "CorrespondingBoundary", options.corresponding_boundary.value_or(express::Base()));
        }
        ifcapi::detail::write_enum_attr(*entity, "PhysicalOrVirtualBoundary", options.physical_or_virtual);
        ifcapi::detail::write_enum_attr(*entity, "InternalOrExternalBoundary", options.internal_or_external);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

} // namespace bindings
} // namespace ifcapi
