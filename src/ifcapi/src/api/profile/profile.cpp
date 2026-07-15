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

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

std::vector<express::Base> direct_entity_attributes(express::Base entity) {
    std::vector<express::Base> result;
    auto* declaration = entity ? entity.declaration().as_entity() : nullptr;
    if (!entity || !declaration) return result;
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = entity.get_attribute_value(i);
            if (!value.isNull() && value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
                result.push_back(static_cast<express::Base>(value));
            }
        } catch (...) {
        }
    }
    return result;
}

std::vector<express::Base> profile_psets(ifcopenshell::file* file, express::Base profile) {
    std::vector<express::Base> result;
    if (!file || !profile) return result;
    if (is_ifc2x3(file)) {
        const auto* declaration = file->schema()->declaration_by_name("IfcProfileProperties");
        auto instances = file->instances_by_type(declaration);
        for (auto pset : instances) {
            if (ifcapi::detail::read_ref_attr(pset, "ProfileDefinition") == profile) {
                result.push_back(pset);
            }
        }
        return result;
    }
    auto inverses = file->instances_by_reference(static_cast<int>(profile.id()));
    for (auto inverse : inverses) {
        if (inverse && inverse.declaration().is("IfcProfileProperties")) {
            result.push_back(inverse);
        }
    }
    return result;
}

express::Base create_arbitrary_profile_curve(
    ifcopenshell::file* file,
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

express::Base profile_add_parameterized_profile(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const std::string& profile_type)
{
    const auto* declaration = file->schema()->declaration_by_name(ifc_class);
    auto result = file->create(declaration);
    ifcapi::detail::write_string_attr(result, "ProfileType", profile_type);
    return result;
}

express::Base profile_add_arbitrary_profile(
    ifcopenshell::file* file,
    const ProfileAddArbitraryProfileOptions& options)
{
    auto points = ifcapi::detail::convert_si_to_project_units(file, options.profile);
    auto curve = create_arbitrary_profile_curve(file, points);
    auto result = file->create(file->schema()->declaration_by_name("IfcArbitraryClosedProfileDef"));
    ifcapi::detail::write_string_attr(result, "ProfileType", "AREA");
    if (options.name) {
        ifcapi::detail::write_string_attr(result, "ProfileName", *options.name);
    }
    ifcapi::detail::write_ref_attr(result, "OuterCurve", curve);
    return result;
}

express::Base profile_add_arbitrary_profile_with_voids(
    ifcopenshell::file* file,
    const ProfileAddArbitraryProfileWithVoidsOptions& options)
{
    auto outer_points = ifcapi::detail::convert_si_to_project_units(file, options.outer_profile);
    auto outer_curve = create_arbitrary_profile_curve(file, outer_points, !is_ifc2x3(file));
    std::vector<express::Base> inner_curves;
    for (const auto& inner_profile : options.inner_profiles) {
        auto inner_points = ifcapi::detail::convert_si_to_project_units(file, inner_profile);
        inner_curves.push_back(create_arbitrary_profile_curve(file, inner_points));
    }
    auto result = file->create(file->schema()->declaration_by_name("IfcArbitraryProfileDefWithVoids"));
    ifcapi::detail::write_string_attr(result, "ProfileType", "AREA");
    if (options.name) {
        ifcapi::detail::write_string_attr(result, "ProfileName", *options.name);
    }
    ifcapi::detail::write_ref_attr(result, "OuterCurve", outer_curve);
    ifcapi::detail::write_ref_aggregate(result, "InnerCurves", inner_curves);
    return result;
}

express::Base profile_copy_profile(
    ifcopenshell::file* file,
    express::Base* profile_ptr)
{
    auto profile = ifcapi::detail::deref_or_empty(profile_ptr);
    auto new_profile = shape_builder_deep_copy(file, &profile);
    auto inverses = file->instances_by_reference(static_cast<int>(profile.id()));
    {
        for (auto inverse : inverses) {
            if (!inverse || !inverse.declaration().is("IfcProfileProperties")) continue;
            auto new_pset = ifcapi::detail::shallow_copy(file, inverse);
            ifcapi::detail::write_ref_attr(new_pset, "ProfileDefinition", new_profile);
        }
    }
    return new_profile;
}

void profile_edit_profile(
    express::Base* profile_ptr,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    auto profile = ifcapi::detail::deref_or_empty(profile_ptr);
    if (!profile || !attributes) {
        ifcapi::detail::set_error("Invalid arguments");
        return;
    }
    try {
        ifcapi::detail::apply_attribute_props(profile, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void profile_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile_ptr)
{
    auto profile = ifcapi::detail::deref_or_empty(profile_ptr);
    auto subelements = direct_entity_attributes(profile);
    auto psets = profile_psets(file, profile);
    for (auto pset : psets) {
        pset_remove_pset(file, &profile, &pset);
    }
    file->remove_entity(profile);
    for (auto subelement : subelements) {
        entity_remove_deep(&subelement);
    }
}

} // namespace bindings
} // namespace ifcapi
