// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/profile.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

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
