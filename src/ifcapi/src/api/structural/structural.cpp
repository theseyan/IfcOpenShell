// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/structural.h"

#include "ifcopenshell_api_internal.hpp"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/detail/attribute.h"

#include <stdexcept>

namespace ifcapi {
namespace bindings {
namespace {

void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

} // namespace

IfcUtil::IfcBaseClass* structural_add_structural_analysis_model(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();
    return root_create_entity(file, "IfcStructuralAnalysisModel", "LOADING_3D", nullptr, owner_history);
}

IfcUtil::IfcBaseClass* structural_assign_structural_analysis_model(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    return group_assign_group(file, products, structural_analysis_model, owner_history, user, application);
}

void structural_unassign_structural_analysis_model(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    group_unassign_group(file, products, structural_analysis_model, user, application);
}

IfcUtil::IfcBaseClass* structural_assign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();
    if (!file || !relating_product || !related_object) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        for (auto* rel : ifcapi::detail::read_inverse_aggregate(relating_product, "ReferencedBy")) {
            if (!rel || !rel->declaration().is("IfcRelAssignsToProduct")) {
                continue;
            }
            auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            if (ifcapi::detail::contains_ref(related_objects, related_object)) {
                return rel;
            }
            related_objects.push_back(related_object);
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            return rel;
        }

        auto* rel = root_create_entity(file, "IfcRelAssignsToProduct", nullptr, nullptr, owner_history);
        if (!rel) {
            return nullptr;
        }
        ifcapi::detail::write_ref_attr(rel, "RelatingProduct", relating_product);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {related_object});
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* structural_add_structural_activity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* applied_load,
    IfcUtil::IfcBaseClass* structural_member,
    const std::string& ifc_class,
    const std::string& predefined_type,
    const std::string& global_or_local,
    IfcUtil::IfcBaseClass* activity_owner_history,
    IfcUtil::IfcBaseClass* relationship_owner_history)
{
    ifcopenshell_clear_error();
    if (!file || !applied_load || !structural_member) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        auto* activity = root_create_entity(file, ifc_class, predefined_type.c_str(), nullptr, activity_owner_history);
        if (!activity) {
            return nullptr;
        }
        ifcapi::detail::write_ref_attr(activity, "AppliedLoad", applied_load);
        ifcapi::detail::write_enum_attr(activity, "GlobalOrLocal", global_or_local);

        auto* rel = root_create_entity(
            file,
            "IfcRelConnectsStructuralActivity",
            nullptr,
            nullptr,
            relationship_owner_history);
        if (!rel) {
            return nullptr;
        }
        ifcapi::detail::write_ref_attr(rel, "RelatingElement", structural_member);
        ifcapi::detail::write_ref_attr(rel, "RelatedStructuralActivity", activity);
        return activity;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

} // namespace bindings
} // namespace ifcapi
