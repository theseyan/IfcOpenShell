// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/structural.h"

#include "ifcopenshell_api_internal.hpp"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"

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

IfcUtil::IfcBaseClass* structural_add_structural_load(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const char* name,
    bool has_name)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("Invalid arguments");
        return nullptr;
    }
    try {
        auto* load = file->create(file->schema()->declaration_by_name(ifc_class));
        if (has_name) {
            ifcapi::detail::write_string_attr(load, "Name", name ? name : "");
        }
        return load;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* structural_add_structural_load_case(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();
    auto* load_case = root_create_entity(file, "IfcStructuralLoadCase", "LOAD_CASE", name.c_str(), owner_history);
    if (load_case) {
        ifcapi::detail::write_enum_attr(load_case, "ActionType", action_type);
        ifcapi::detail::write_enum_attr(load_case, "ActionSource", action_source);
    }
    return load_case;
}

IfcUtil::IfcBaseClass* structural_add_structural_load_group(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();
    auto* load_group = root_create_entity(file, "IfcStructuralLoadGroup", "LOAD_GROUP", name.c_str(), owner_history);
    if (load_group) {
        ifcapi::detail::write_enum_attr(load_group, "ActionType", action_type);
        ifcapi::detail::write_enum_attr(load_group, "ActionSource", action_source);
    }
    return load_group;
}

IfcUtil::IfcBaseClass* structural_add_structural_member_connection(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_structural_member,
    IfcUtil::IfcBaseClass* related_structural_connection,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();
    if (!file || !relating_structural_member || !related_structural_connection) {
        set_error("Invalid arguments");
        return nullptr;
    }
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(
             related_structural_connection,
             "ConnectsStructuralMembers")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatingStructuralMember") == relating_structural_member) {
            return rel;
        }
    }
    auto* rel = root_create_entity(file, "IfcRelConnectsStructuralMember", nullptr, nullptr, owner_history);
    if (!rel) {
        return nullptr;
    }
    ifcapi::detail::write_ref_attr(rel, "RelatingStructuralMember", relating_structural_member);
    ifcapi::detail::write_ref_attr(rel, "RelatedStructuralConnection", related_structural_connection);
    return rel;
}

IfcUtil::IfcBaseClass* structural_assign_to_building(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* building,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();
    if (!file || !structural_analysis_model || !building) {
        set_error("Invalid arguments");
        return nullptr;
    }
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(structural_analysis_model, "ServicesBuildings")) {
        auto buildings = ifcapi::detail::read_ref_aggregate(rel, "RelatedBuildings");
        if (ifcapi::detail::contains_ref(buildings, building)) {
            return rel;
        }
        buildings.push_back(building);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedBuildings", buildings);
        return rel;
    }
    auto* rel = root_create_entity(file, "IfcRelServicesBuildings", nullptr, nullptr, owner_history);
    if (!rel) {
        return nullptr;
    }
    ifcapi::detail::write_ref_attr(rel, "RelatingSystem", structural_analysis_model);
    ifcapi::detail::write_ref_aggregate(rel, "RelatedBuildings", {building});
    return rel;
}

void structural_remove_structural_analysis_model(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_analysis_model)
{
    ifcopenshell_clear_error();
    if (!file || !structural_analysis_model) return;
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(structural_analysis_model, "IsGroupedBy")) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    ifcapi::detail::remove_with_history(file, structural_analysis_model);
}

void structural_remove_structural_load(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* structural_load) {
    ifcopenshell_clear_error();
    if (file && structural_load) {
        file->removeEntity(structural_load);
    }
}

void structural_remove_structural_load_case(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* structural_load_case) {
    ifcopenshell_clear_error();
    if (!file || !structural_load_case) return;
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(structural_load_case, "IsGroupedBy")) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    ifcapi::detail::remove_with_history(file, structural_load_case);
}

void structural_remove_structural_load_group(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* structural_load_group) {
    ifcopenshell_clear_error();
    if (!file || !structural_load_group) return;
    auto inverses = file->getInverse(structural_load_group->id(), nullptr, -1);
    if (inverses) {
        std::vector<IfcUtil::IfcBaseClass*> rels;
        for (auto* inverse : *inverses) {
            if (inverse && inverse->declaration().is("IfcRelAssignsToGroup")
                && ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() == 1) {
                rels.push_back(inverse);
            }
        }
        for (auto* rel : rels) {
            ifcapi::detail::remove_with_history(file, rel);
        }
    }
    ifcapi::detail::remove_with_history(file, structural_load_group);
}

} // namespace bindings
} // namespace ifcapi
