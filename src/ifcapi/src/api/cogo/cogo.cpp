// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/cogo.h"

#include "guid.h"
#include "ifcopenshell_api_internal.hpp"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/detail/attribute.h"

#include <stdexcept>
#include <cctype>
#include <cmath>
#include <sstream>
#include <string>
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

express::Base world_coordinate_system(express::Base context) {
    std::set<int> seen;
    while (context && is_a(context, "IfcGeometricRepresentationSubContext")) {
        auto parent = ifcapi::detail::read_ref_attr(context, "ParentContext");
        if (!parent || !seen.insert(parent.id()).second) {
            break;
        }
        context = parent;
    }
    if (!context) {
        return {};
    }
    return ifcapi::detail::read_ref_attr(context, "WorldCoordinateSystem");
}

express::Base first_item(express::Base annotation) {
    auto items = ifcapi::detail::read_ref_aggregate(first_representation(annotation), "Items");
    return items.empty() ? express::Base() : items.front();
}

int parse_integer_token(const std::string& token) {
    if (token.empty()) throw std::invalid_argument("Invalid bearing string");
    size_t consumed = 0;
    int value = 0;
    try {
        value = std::stoi(token, &consumed);
    } catch (...) {
        throw std::invalid_argument("Invalid bearing string");
    }
    if (consumed != token.size()) throw std::invalid_argument("Invalid bearing string");
    return value;
}

double parse_seconds_token(const std::string& token) {
    if (token.empty()) throw std::invalid_argument("Invalid bearing string");
    size_t consumed = 0;
    double value = 0.0;
    try {
        value = std::stod(token, &consumed);
    } catch (...) {
        throw std::invalid_argument("Invalid bearing string");
    }
    if (consumed != token.size() || !std::isfinite(value)) {
        throw std::invalid_argument("Invalid bearing string");
    }
    return value;
}

} // namespace

double cogo_bearing2dd(const std::string& bearing) {
    std::istringstream input(bearing);
    std::vector<std::string> parts;
    for (std::string part; input >> part;) parts.push_back(part);
    if (parts.size() < 3 || parts.size() > 5 || parts.front().size() != 1 || parts.back().size() != 1) {
        throw std::invalid_argument("Invalid bearing string");
    }

    const char north_south = static_cast<char>(std::toupper(static_cast<unsigned char>(parts.front()[0])));
    const char east_west = static_cast<char>(std::toupper(static_cast<unsigned char>(parts.back()[0])));
    if ((north_south != 'N' && north_south != 'S') || (east_west != 'E' && east_west != 'W')) {
        throw std::invalid_argument("Invalid bearing string");
    }

    const int degrees = parse_integer_token(parts[1]);
    const int minutes = parts.size() >= 4 ? parse_integer_token(parts[2]) : 0;
    const double seconds_value = parts.size() == 5 ? parse_seconds_token(parts[3]) : 0.0;
    const int seconds = static_cast<int>(seconds_value);
    const double hundredths = 100.0 * (seconds_value - seconds);
    if (degrees < 0 || minutes < 0 || minutes >= 60 || seconds_value < 0.0 || seconds >= 60 || hundredths < 0.0) {
        throw std::invalid_argument("Invalid bearing string");
    }

    const double quadrant = static_cast<double>(degrees) + static_cast<double>(minutes) / 60.0
        + static_cast<double>(seconds) / 3600.0 + hundredths / 3600000.0;
    if (!std::isfinite(quadrant) || quadrant > 90.0) {
        throw std::invalid_argument("Invalid bearing string");
    }

    double angle = north_south == 'N' ? 90.0 : 270.0;
    const double sign = (north_south == 'N') == (east_west == 'E') ? -1.0 : 1.0;
    angle += sign * quadrant;
    return angle == 360.0 ? 0.0 : angle;
}

express::Base cogo_add_survey_point(
    ifcopenshell::file* file,
    const CogoAddSurveyPointOptions& options)
{
    ifcopenshell_clear_error();
    if (!file || !options.survey_point) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto context = find_context(file);
        if (!context) {
            throw std::runtime_error("No Model/Annotation/MODEL_VIEW representation context found");
        }
        auto actual_site = options.site.value_or(first_site(file));
        if (!actual_site) {
            throw std::runtime_error("No IfcSite found");
        }

        auto shape_representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        ifcapi::detail::write_ref_attr(shape_representation, "ContextOfItems", context);
        ifcapi::detail::write_string_attr(shape_representation, "RepresentationIdentifier", "Annotation");
        ifcapi::detail::write_string_attr(shape_representation, "RepresentationType", "Point");
        ifcapi::detail::write_ref_aggregate(shape_representation, "Items", {options.survey_point});

        auto representation = file->create(file->schema()->declaration_by_name("IfcProductDefinitionShape"));
        ifcapi::detail::write_ref_aggregate(representation, "Representations", {shape_representation});

        auto annotation = file->create(file->schema()->declaration_by_name("IfcAnnotation"));
        ifcapi::detail::write_string_attr(annotation, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            annotation,
            "ObjectPlacement",
            world_coordinate_system(context));
        ifcapi::detail::write_ref_attr(annotation, "Representation", representation);
        ifcapi::detail::write_enum_attr(annotation, "PredefinedType", "SURVEY");

        spatial_assign_container(file, {
            {annotation},
            actual_site,
            options.owner_history,
            options.user,
            options.application,
        });
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
