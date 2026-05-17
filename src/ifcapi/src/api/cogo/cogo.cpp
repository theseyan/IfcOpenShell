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

bool is_a(IfcUtil::IfcBaseClass* entity, const char* name) {
    return entity && entity->declaration().is(name);
}

IfcUtil::IfcBaseClass* find_context(IfcParse::IfcFile* file) {
    auto contexts = file->instances_by_type(std::string("IfcGeometricRepresentationContext"));
    if (!contexts) {
        return nullptr;
    }
    for (auto it = contexts->begin(); it != contexts->end(); ++it) {
        auto* context = *it;
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
    return nullptr;
}

IfcUtil::IfcBaseClass* first_site(IfcParse::IfcFile* file) {
    auto sites = file->instances_by_type(std::string("IfcSite"));
    if (!sites || sites->begin() == sites->end()) {
        return nullptr;
    }
    return *sites->begin();
}

IfcUtil::IfcBaseClass* first_representation(IfcUtil::IfcBaseClass* annotation) {
    auto* product_representation = ifcapi::detail::read_ref_attr(annotation, "Representation");
    auto representations = ifcapi::detail::read_ref_aggregate(product_representation, "Representations");
    return representations.empty() ? nullptr : representations.front();
}

IfcUtil::IfcBaseClass* first_item(IfcUtil::IfcBaseClass* annotation) {
    auto items = ifcapi::detail::read_ref_aggregate(first_representation(annotation), "Items");
    return items.empty() ? nullptr : items.front();
}

} // namespace

IfcUtil::IfcBaseClass* cogo_add_survey_point(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* survey_point,
    IfcUtil::IfcBaseClass* site,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file || !survey_point) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        auto* context = find_context(file);
        if (!context) {
            throw std::runtime_error("No Model/Annotation/MODEL_VIEW representation context found");
        }
        auto* actual_site = site ? site : first_site(file);
        if (!actual_site) {
            throw std::runtime_error("No IfcSite found");
        }

        auto* shape_representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        ifcapi::detail::write_ref_attr(shape_representation, "ContextOfItems", context);
        ifcapi::detail::write_string_attr(shape_representation, "RepresentationIdentifier", "Annotation");
        ifcapi::detail::write_string_attr(shape_representation, "RepresentationType", "Point");
        ifcapi::detail::write_ref_aggregate(shape_representation, "Items", {survey_point});

        auto* representation = file->create(file->schema()->declaration_by_name("IfcProductDefinitionShape"));
        ifcapi::detail::write_ref_aggregate(representation, "Representations", {shape_representation});

        auto* annotation = file->create(file->schema()->declaration_by_name("IfcAnnotation"));
        ifcapi::detail::write_string_attr(annotation, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            annotation,
            "ObjectPlacement",
            ifcapi::detail::read_ref_attr(context, "WorldCoordinateSystem"));
        ifcapi::detail::write_ref_attr(annotation, "Representation", representation);
        ifcapi::detail::write_enum_attr(annotation, "PredefinedType", "SURVEY");

        std::vector<const IfcUtil::IfcBaseClass*> products = {annotation};
        spatial_assign_container(file, products, actual_site, owner_history, user, application);
        return annotation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void cogo_assign_survey_point(IfcUtil::IfcBaseClass* annotation, IfcUtil::IfcBaseClass* survey_point) {
    ifcopenshell_clear_error();
    if (!annotation || !survey_point) {
        set_error("Invalid arguments");
        return;
    }

    try {
        auto* representation = first_representation(annotation);
        if (!representation) {
            throw std::runtime_error("Annotation has no shape representation");
        }
        ifcapi::detail::write_ref_aggregate(representation, "Items", {survey_point});
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void cogo_edit_survey_point(IfcUtil::IfcBaseClass* annotation, double x, double y, double z) {
    ifcopenshell_clear_error();
    if (!annotation) {
        set_error("Invalid arguments");
        return;
    }

    try {
        auto* survey_point = first_item(annotation);
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
