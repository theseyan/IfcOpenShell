// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/cogo.h"

#include "guid.h"
#include "ifcopenshell_api_internal.hpp"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/detail/attribute.h"

#include <stdexcept>
#include <vector>

namespace ifcapi {
namespace bindings {
namespace {

void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

bool is_a(express::Base entity, const char* name) {
    return entity && entity.declaration().is(name);
}

express::Base find_context(ifcopenshell::file* file) {
    auto* declaration = ifcapi::detail::declaration_by_name(file, "IfcGeometricRepresentationContext");
    if (!declaration) return {};
    for (auto context : file->instances_by_type(declaration)) {
        if (ifcapi::detail::read_string_attr(context, "ContextType") != "Model") {
            continue;
        }
        if (ifcapi::detail::read_string_attr(context, "ContextIdentifier") != "Annotation") {
            continue;
        }
        if (is_a(context, "IfcGeometricRepresentationSubContext")
            && ifcapi::detail::read_string_attr(context, "TargetView") != "MODEL_VIEW") {
            continue;
        }
        return context;
    }
    return {};
}

express::Base first_site(ifcopenshell::file* file) {
    auto sites = ifcapi::detail::instances_by_type(file, "IfcSite");
    return sites.empty() ? express::Base() : sites.front();
}

express::Base first_representation(express::Base annotation) {
    auto product_representation = ifcapi::detail::read_ref_attr(annotation, "Representation");
    auto representations = ifcapi::detail::read_ref_aggregate(product_representation, "Representations");
    return representations.empty() ? express::Base() : representations.front();
}

express::Base first_item(express::Base annotation) {
    auto items = ifcapi::detail::read_ref_aggregate(first_representation(annotation), "Items");
    return items.empty() ? express::Base() : items.front();
}

} // namespace

express::Base cogo_add_survey_point(
    ifcopenshell::file* file,
    express::Base* survey_point,
    express::Base* site,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    ifcopenshell_clear_error();
    auto survey_point_value = ifcapi::detail::deref_or_empty(survey_point);
    if (!file || !survey_point_value) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto context = find_context(file);
        if (!context) {
            throw std::runtime_error("No Model/Annotation/MODEL_VIEW representation context found");
        }
        auto actual_site = site && *site ? *site : first_site(file);
        if (!actual_site) {
            throw std::runtime_error("No IfcSite found");
        }

        auto shape_representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        ifcapi::detail::write_ref_attr(shape_representation, "ContextOfItems", context);
        ifcapi::detail::write_string_attr(shape_representation, "RepresentationIdentifier", "Annotation");
        ifcapi::detail::write_string_attr(shape_representation, "RepresentationType", "Point");
        ifcapi::detail::write_ref_aggregate(shape_representation, "Items", {survey_point_value});

        auto representation = file->create(file->schema()->declaration_by_name("IfcProductDefinitionShape"));
        ifcapi::detail::write_ref_aggregate(representation, "Representations", {shape_representation});

        auto annotation = file->create(file->schema()->declaration_by_name("IfcAnnotation"));
        ifcapi::detail::write_string_attr(annotation, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            annotation,
            "ObjectPlacement",
            ifcapi::detail::read_ref_attr(context, "WorldCoordinateSystem"));
        ifcapi::detail::write_ref_attr(annotation, "Representation", representation);
        ifcapi::detail::write_enum_attr(annotation, "PredefinedType", "SURVEY");

        spatial_assign_container(file, {annotation}, &actual_site, owner_history, user, application);
        return annotation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void cogo_assign_survey_point(express::Base* annotation, express::Base* survey_point) {
    ifcopenshell_clear_error();
    auto annotation_value = ifcapi::detail::deref_or_empty(annotation);
    auto survey_point_value = ifcapi::detail::deref_or_empty(survey_point);
    if (!annotation_value || !survey_point_value) {
        set_error("Invalid arguments");
        return;
    }

    try {
        auto representation = first_representation(annotation_value);
        if (!representation) {
            throw std::runtime_error("Annotation has no shape representation");
        }
        ifcapi::detail::write_ref_aggregate(representation, "Items", {survey_point_value});
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void cogo_edit_survey_point(express::Base* annotation, double x, double y, double z) {
    ifcopenshell_clear_error();
    auto annotation_value = ifcapi::detail::deref_or_empty(annotation);
    if (!annotation_value) {
        set_error("Invalid arguments");
        return;
    }

    try {
        auto survey_point = first_item(annotation_value);
        if (!survey_point) {
            throw std::runtime_error("Annotation has no survey point");
        }
        auto coordinates = ifcapi::detail::read_double_aggregate(survey_point, "Coordinates");
        if (coordinates.size() == 2) {
            ifcapi::detail::write_double_aggregate(survey_point, "Coordinates", {x, y});
        } else {
            ifcapi::detail::write_double_aggregate(survey_point, "Coordinates", {x, y, z});
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
