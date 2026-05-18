// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/context.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/detail/attribute.h"
#include "../pset/attribute_props.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcFile.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace {

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

void write_optional_string(IfcUtil::IfcBaseClass* entity, const char* attr, const char* value) {
    if (value) {
        ifcapi::detail::write_string_attr(entity, attr, value);
    }
}

void write_optional_enum(IfcUtil::IfcBaseClass* entity, const char* attr, const char* value) {
    if (!value) {
        return;
    }
    if (!ifcapi::detail::write_enum_attr(entity, attr, value)) {
        throw std::runtime_error(std::string("Unable to find keyword in schema: ") + value);
    }
}

IfcUtil::IfcBaseClass* create_cartesian_point(IfcParse::IfcFile* file, const std::vector<double>& coordinates) {
    auto* point = create_entity(file, "IfcCartesianPoint");
    ifcapi::detail::write_double_aggregate(point, "Coordinates", coordinates);
    return point;
}

IfcUtil::IfcBaseClass* create_direction(IfcParse::IfcFile* file, const std::vector<double>& ratios) {
    auto* direction = create_entity(file, "IfcDirection");
    ifcapi::detail::write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

IfcUtil::IfcBaseClass* create_3d_origin(IfcParse::IfcFile* file) {
    auto* origin = create_entity(file, "IfcAxis2Placement3D");
    ifcapi::detail::write_ref_attr(origin, "Location", create_cartesian_point(file, {0.0, 0.0, 0.0}));
    ifcapi::detail::write_ref_attr(origin, "Axis", create_direction(file, {0.0, 0.0, 1.0}));
    ifcapi::detail::write_ref_attr(origin, "RefDirection", create_direction(file, {1.0, 0.0, 0.0}));
    return origin;
}

IfcUtil::IfcBaseClass* create_2d_origin(IfcParse::IfcFile* file) {
    auto* origin = create_entity(file, "IfcAxis2Placement2D");
    ifcapi::detail::write_ref_attr(origin, "Location", create_cartesian_point(file, {0.0, 0.0}));
    ifcapi::detail::write_ref_attr(origin, "RefDirection", create_direction(file, {1.0, 0.0}));
    return origin;
}

IfcUtil::IfcBaseClass* first_project(IfcParse::IfcFile* file) {
    auto projects = ifcapi::detail::instances_by_type(file, "IfcProject");
    if (projects.empty()) {
        throw std::out_of_range("No IfcProject found");
    }
    return projects.front();
}

void remove_context_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* context) {
    auto subcontexts = ifcapi::detail::read_inverse_aggregate(context, "HasSubContexts");
    for (auto* subcontext : subcontexts) {
        remove_context_impl(file, subcontext);
    }

    auto* parent = ifcapi::detail::read_ref_attr(context, "ParentContext");
    if (parent) {
        auto inverses = file->getInverse(context->id(), nullptr, -1);
        std::vector<IfcUtil::IfcBaseClass*> snapshot;
        if (inverses) {
            for (auto* inverse : *inverses) {
                if (inverse) snapshot.push_back(inverse);
            }
        }
        for (auto* inverse : snapshot) {
            if (inverse->declaration().is("IfcCoordinateOperation")) {
                ifcapi::detail::write_ref_attr(
                    inverse,
                    "SourceCRS",
                    ifcapi::detail::read_ref_attr(inverse, "TargetCRS"));
                ifcapi::bindings::entity_remove_deep2(inverse);
            } else {
                ifcapi::detail::replace_attribute_reference(inverse, context, parent);
            }
        }
        file->removeEntity(context);
        return;
    }

    auto representations = ifcapi::detail::read_inverse_aggregate(context, "RepresentationsInContext");
    file->removeEntity(context);
    for (auto* representation : representations) {
        auto elements = ifcapi::bindings::element_get_elements_by_representation(representation);
        if (elements) {
            for (auto* element : *elements) {
                ifcapi::bindings::geometry_unassign_representation(file, element, representation);
            }
        }
        ifcapi::bindings::geometry_remove_representation(file, representation, true);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* context_add_context(
    IfcParse::IfcFile* file,
    const char* context_type,
    const char* context_identifier,
    const char* target_view,
    bool has_target_scale,
    double target_scale,
    IfcUtil::IfcBaseClass* parent)
{
    if (!parent) {
        auto* context = create_entity(file, "IfcGeometricRepresentationContext");
        if (context_type && std::string(context_type) == "Plan") {
            ifcapi::detail::write_string_attr(context, "ContextType", "Plan");
            ifcapi::detail::write_int_attr(context, "CoordinateSpaceDimension", 2);
            ifcapi::detail::write_ref_attr(context, "WorldCoordinateSystem", create_2d_origin(file));
        } else {
            write_optional_string(context, "ContextType", context_type);
            ifcapi::detail::write_int_attr(context, "CoordinateSpaceDimension", 3);
            ifcapi::detail::write_ref_attr(context, "WorldCoordinateSystem", create_3d_origin(file));
        }
        ifcapi::detail::write_double_attr(context, "Precision", 1.0e-05);

        auto* project = first_project(file);
        auto contexts = ifcapi::detail::read_ref_aggregate(project, "RepresentationContexts");
        contexts.push_back(context);
        ifcapi::detail::write_ref_aggregate(project, "RepresentationContexts", contexts);
        return context;
    }

    auto* subcontext = create_entity(file, "IfcGeometricRepresentationSubContext");
    write_optional_string(subcontext, "ContextIdentifier", context_identifier);
    write_optional_string(subcontext, "ContextType", context_type);
    ifcapi::detail::write_ref_attr(subcontext, "ParentContext", parent);
    write_optional_enum(subcontext, "TargetView", target_view);
    if (has_target_scale) {
        ifcapi::detail::write_double_attr(subcontext, "TargetScale", target_scale);
    }
    return subcontext;
}

void context_remove_context(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* context) {
    if (!file || !context) return;
    remove_context_impl(file, context);
}

void context_edit_context(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* context, ifcopenshell_pset_props_t* attributes) {
    (void)file;
    ifcopenshell_clear_error();
    try {
        if (!context) {
            throw std::runtime_error("context_edit_context requires a context");
        }
        ifcapi::detail::apply_attribute_props(context, attributes);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
