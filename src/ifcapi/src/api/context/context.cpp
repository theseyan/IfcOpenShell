// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/context.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/detail/attribute.h"
#include "../pset/attribute_props.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/file.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace {

express::Base create_entity(ifcopenshell::file* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

void write_optional_string(express::Base entity, const char* attr, const char* value) {
    if (value) {
        ifcapi::detail::write_string_attr(entity, attr, value);
    }
}

void write_optional_enum(express::Base entity, const char* attr, const char* value) {
    if (!value) {
        return;
    }
    if (!ifcapi::detail::write_enum_attr(entity, attr, value)) {
        throw std::runtime_error(std::string("Unable to find keyword in schema: ") + value);
    }
}

express::Base create_cartesian_point(ifcopenshell::file* file, const std::vector<double>& coordinates) {
    auto point = create_entity(file, "IfcCartesianPoint");
    ifcapi::detail::write_double_aggregate(point, "Coordinates", coordinates);
    return point;
}

express::Base create_direction(ifcopenshell::file* file, const std::vector<double>& ratios) {
    auto direction = create_entity(file, "IfcDirection");
    ifcapi::detail::write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

express::Base create_3d_origin(ifcopenshell::file* file) {
    auto origin = create_entity(file, "IfcAxis2Placement3D");
    ifcapi::detail::write_ref_attr(origin, "Location", create_cartesian_point(file, {0.0, 0.0, 0.0}));
    ifcapi::detail::write_ref_attr(origin, "Axis", create_direction(file, {0.0, 0.0, 1.0}));
    ifcapi::detail::write_ref_attr(origin, "RefDirection", create_direction(file, {1.0, 0.0, 0.0}));
    return origin;
}

express::Base create_2d_origin(ifcopenshell::file* file) {
    auto origin = create_entity(file, "IfcAxis2Placement2D");
    ifcapi::detail::write_ref_attr(origin, "Location", create_cartesian_point(file, {0.0, 0.0}));
    ifcapi::detail::write_ref_attr(origin, "RefDirection", create_direction(file, {1.0, 0.0}));
    return origin;
}

express::Base first_project(ifcopenshell::file* file) {
    auto projects = ifcapi::detail::instances_by_type(file, "IfcProject");
    if (projects.empty()) {
        throw std::out_of_range("No IfcProject found");
    }
    return projects.front();
}

void remove_context_impl(ifcopenshell::file* file, express::Base context) {
    auto subcontexts = ifcapi::detail::read_inverse_aggregate(context, "HasSubContexts");
    for (auto subcontext : subcontexts) {
        remove_context_impl(file, subcontext);
    }

    auto parent = ifcapi::detail::read_ref_attr(context, "ParentContext");
    if (parent) {
        std::vector<express::Base> snapshot;
        for (auto inverse : file->instances_by_reference(static_cast<int>(context.id()))) {
            if (inverse) snapshot.push_back(inverse);
        }
        for (auto inverse : snapshot) {
            if (inverse.declaration().is("IfcCoordinateOperation")) {
                ifcapi::detail::write_ref_attr(
                    inverse,
                    "SourceCRS",
                    ifcapi::detail::read_ref_attr(inverse, "TargetCRS"));
                ifcapi::bindings::entity_remove_deep(&inverse);
            } else {
                ifcapi::detail::replace_attribute_reference(inverse, context, parent);
            }
        }
        file->remove_entity(context);
        return;
    }

    auto representations = ifcapi::detail::read_inverse_aggregate(context, "RepresentationsInContext");
    file->remove_entity(context);
    for (auto representation : representations) {
        auto elements = ifcapi::bindings::element_get_elements_by_representation(&representation);
        for (auto element : elements) {
            ifcapi::bindings::geometry_unassign_representation(file, &element, &representation);
        }
        ifcapi::bindings::geometry_remove_representation(
            file,
            &representation,
            ifcapi::bindings::GeometryRemoveRepresentationOptions{true});
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base context_add_context(
    ifcopenshell::file* file,
    const ContextAddContextOptions& options)
{
    auto parent_value = options.parent.value_or(express::Base());
    if (!parent_value) {
        auto context = create_entity(file, "IfcGeometricRepresentationContext");
        if (!options.context_type.empty() && options.context_type == "Plan") {
            ifcapi::detail::write_string_attr(context, "ContextType", "Plan");
            ifcapi::detail::write_int_attr(context, "CoordinateSpaceDimension", 2);
            ifcapi::detail::write_ref_attr(context, "WorldCoordinateSystem", create_2d_origin(file));
        } else {
            if (!options.context_type.empty()) {
                ifcapi::detail::write_string_attr(context, "ContextType", options.context_type);
            }
            ifcapi::detail::write_int_attr(context, "CoordinateSpaceDimension", 3);
            ifcapi::detail::write_ref_attr(context, "WorldCoordinateSystem", create_3d_origin(file));
        }
        ifcapi::detail::write_double_attr(context, "Precision", 1.0e-05);

        auto project = first_project(file);
        auto contexts = ifcapi::detail::read_ref_aggregate(project, "RepresentationContexts");
        contexts.push_back(context);
        ifcapi::detail::write_ref_aggregate(project, "RepresentationContexts", contexts);
        return context;
    }

    auto subcontext = create_entity(file, "IfcGeometricRepresentationSubContext");
    if (!options.context_identifier.empty()) {
        ifcapi::detail::write_string_attr(subcontext, "ContextIdentifier", options.context_identifier);
    }
    if (!options.context_type.empty()) {
        ifcapi::detail::write_string_attr(subcontext, "ContextType", options.context_type);
    }
    ifcapi::detail::write_ref_attr(subcontext, "ParentContext", parent_value);
    if (!options.target_view.empty()) {
        if (!ifcapi::detail::write_enum_attr(subcontext, "TargetView", options.target_view)) {
            throw std::runtime_error(std::string("Unable to find keyword in schema: ") + options.target_view);
        }
    }
    if (options.target_scale) {
        ifcapi::detail::write_double_attr(subcontext, "TargetScale", *options.target_scale);
    }
    return subcontext;
}

void context_remove_context(ifcopenshell::file* file, express::Base* context) {
    auto context_value = ifcapi::detail::deref_or_empty(context);
    if (!file || !context_value) return;
    remove_context_impl(file, context_value);
}

void context_edit_context(ifcopenshell::file* file, express::Base* context, ifcopenshell_pset_props_t* attributes) {
    (void)file;
    ifcopenshell_clear_error();
    try {
        auto context_value = ifcapi::detail::deref_or_empty(context);
        if (!context_value) {
            throw std::runtime_error("context_edit_context requires a context");
        }
        ifcapi::detail::apply_attribute_props(context_value, attributes);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
