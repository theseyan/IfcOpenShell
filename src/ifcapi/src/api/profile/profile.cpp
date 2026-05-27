// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/profile.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/geometry.h"
#include "../pset/attribute_props.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

std::vector<IfcUtil::IfcBaseClass*> direct_entity_attributes(IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* declaration = entity ? entity->declaration().as_entity() : nullptr;
    if (!entity || !declaration) return result;
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = entity->get_attribute_value(i);
            if (!value.isNull() && value.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
                result.push_back(static_cast<IfcUtil::IfcBaseClass*>(value));
            }
        } catch (...) {
        }
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> profile_psets(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* profile) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !profile) return result;
    if (is_ifc2x3(file)) {
        const auto* declaration = file->schema()->declaration_by_name("IfcProfileProperties");
        auto instances = file->instances_by_type(declaration);
        if (!instances) return result;
        for (auto* pset : *instances) {
            if (ifcapi::detail::read_ref_attr(pset, "ProfileDefinition") == profile) {
                result.push_back(pset);
            }
        }
        return result;
    }
    auto inverses = file->getInverse(profile->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto* inverse : *inverses) {
        if (inverse && inverse->declaration().is("IfcProfileProperties")) {
            result.push_back(inverse);
        }
    }
    return result;
}

IfcUtil::IfcBaseClass* create_arbitrary_profile_curve(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& points,
    bool force_3d_point_list = false)
{
    size_t dimensions = force_3d_point_list ? 3 : points.at(0).size();
    if (!force_3d_point_list && dimensions != 2 && dimensions != 3) {
        throw std::runtime_error("Invalid dimensions: " + std::to_string(dimensions) + ".");
    }
    return ifcapi::detail::create_polyline_or_indexed_polycurve(file, points, dimensions, false);
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* profile_add_parameterized_profile(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const std::string& profile_type)
{
    const auto* declaration = file->schema()->declaration_by_name(ifc_class);
    auto* result = file->create(declaration);
    ifcapi::detail::write_string_attr(result, "ProfileType", profile_type);
    return result;
}

IfcUtil::IfcBaseClass* profile_add_arbitrary_profile(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& profile,
    const char* name,
    bool has_name)
{
    auto points = ifcapi::detail::convert_si_to_project_units(file, profile);
    auto* curve = create_arbitrary_profile_curve(file, points);
    auto* result = file->create(file->schema()->declaration_by_name("IfcArbitraryClosedProfileDef"));
    ifcapi::detail::write_string_attr(result, "ProfileType", "AREA");
    if (has_name) {
        ifcapi::detail::write_string_attr(result, "ProfileName", name ? name : "");
    }
    ifcapi::detail::write_ref_attr(result, "OuterCurve", curve);
    return result;
}

IfcUtil::IfcBaseClass* profile_add_arbitrary_profile_with_voids(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& outer_profile,
    const std::vector<std::vector<std::vector<double>>>& inner_profiles,
    const char* name,
    bool has_name)
{
    auto outer_points = ifcapi::detail::convert_si_to_project_units(file, outer_profile);
    auto* outer_curve = create_arbitrary_profile_curve(file, outer_points, !is_ifc2x3(file));
    std::vector<IfcUtil::IfcBaseClass*> inner_curves;
    for (const auto& inner_profile : inner_profiles) {
        auto inner_points = ifcapi::detail::convert_si_to_project_units(file, inner_profile);
        inner_curves.push_back(create_arbitrary_profile_curve(file, inner_points));
    }
    auto* result = file->create(file->schema()->declaration_by_name("IfcArbitraryProfileDefWithVoids"));
    ifcapi::detail::write_string_attr(result, "ProfileType", "AREA");
    if (has_name) {
        ifcapi::detail::write_string_attr(result, "ProfileName", name ? name : "");
    }
    ifcapi::detail::write_ref_attr(result, "OuterCurve", outer_curve);
    ifcapi::detail::write_ref_aggregate(result, "InnerCurves", inner_curves);
    return result;
}

IfcUtil::IfcBaseClass* profile_copy_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile)
{
    auto* new_profile = shape_builder_deep_copy(file, profile);
    auto inverses = file->getInverse(profile->id(), nullptr, -1);
    if (inverses) {
        for (auto* inverse : *inverses) {
            if (!inverse || !inverse->declaration().is("IfcProfileProperties")) continue;
            auto* new_pset = ifcapi::detail::shallow_copy(file, inverse);
            ifcapi::detail::write_ref_attr(new_pset, "ProfileDefinition", new_profile);
        }
    }
    return new_profile;
}

void profile_edit_profile(
    IfcUtil::IfcBaseClass* profile,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    if (!profile || !attributes) {
        ifcapi::detail::set_error("Invalid arguments");
        return;
    }
    try {
        ifcapi::detail::apply_attribute_props(profile, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void profile_remove_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile)
{
    auto subelements = direct_entity_attributes(profile);
    auto psets = profile_psets(file, profile);
    for (auto* pset : psets) {
        pset_remove_pset(file, profile, pset);
    }
    file->removeEntity(profile);
    for (auto* subelement : subelements) {
        entity_remove_deep2(subelement);
    }
}

} // namespace bindings
} // namespace ifcapi
