// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/structural.h"

#include "ifcopenshell_api_internal.hpp"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "../pset/attribute_props.hpp"

#include <stdexcept>

namespace ifcapi {
namespace bindings {
namespace {

void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity || !entity->id()) return 0;
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    return inverses ? inverses->size() : 0;
}

IfcUtil::IfcBaseClass* create_direction(IfcParse::IfcFile* file, const std::vector<double>& ratios) {
    if (ratios.size() != 3) {
        throw std::runtime_error("Expected a 3D direction vector");
    }
    auto* direction = file->create(file->schema()->declaration_by_name("IfcDirection"));
    ifcapi::detail::write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

IfcUtil::IfcBaseClass* create_condition_coordinate_system(IfcParse::IfcFile* file) {
    auto* point = file->create(file->schema()->declaration_by_name("IfcCartesianPoint"));
    ifcapi::detail::write_double_aggregate(point, "Coordinates", {0.0, 0.0, 0.0});
    auto* placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    ifcapi::detail::write_ref_attr(placement, "Location", point);
    return placement;
}

const ifcapi_pset::Entry* nested_entry(const ifcapi_pset::Entry& entry, const std::string& key) {
    if (!entry.nested) return nullptr;
    for (const auto& nested : entry.nested->entries) {
        if (nested.key == key) return &nested;
    }
    return nullptr;
}

std::string entry_string(const ifcapi_pset::Entry& entry) {
    switch (entry.kind) {
        case ifcapi_pset::Kind::STRING:
        case ifcapi_pset::Kind::TYPED_STRING:
            return entry.s_val;
        default:
            throw std::runtime_error("Expected a string value");
    }
}

double entry_double(const ifcapi_pset::Entry& entry) {
    switch (entry.kind) {
        case ifcapi_pset::Kind::DOUBLE:
        case ifcapi_pset::Kind::TYPED_DOUBLE:
            return entry.d_val;
        case ifcapi_pset::Kind::INT:
        case ifcapi_pset::Kind::TYPED_INT:
            return static_cast<double>(entry.i_val);
        case ifcapi_pset::Kind::BOOL:
        case ifcapi_pset::Kind::TYPED_BOOL:
            return entry.b_val ? 1.0 : 0.0;
        default:
            throw std::runtime_error("Expected a numeric value");
    }
}

IfcUtil::IfcBaseClass* create_typed_value(
    IfcParse::IfcFile* file,
    const std::string& ifc_type,
    const ifcapi_pset::Entry& value)
{
    auto* declaration = file->schema()->declaration_by_name(ifc_type);
    auto* type_declaration = declaration ? declaration->as_type_declaration() : nullptr;
    if (!type_declaration) {
        throw std::runtime_error("Unable to create " + ifc_type);
    }
    auto* result = file->create(type_declaration);
    if (ifc_type == "IfcBoolean") {
        if (value.kind == ifcapi_pset::Kind::BOOL || value.kind == ifcapi_pset::Kind::TYPED_BOOL) {
            result->set_attribute_value(0, value.b_val);
        } else {
            throw std::runtime_error("IfcBoolean requires a boolean value");
        }
    } else if (value.kind == ifcapi_pset::Kind::STRING || value.kind == ifcapi_pset::Kind::TYPED_STRING) {
        result->set_attribute_value(0, value.s_val);
    } else {
        result->set_attribute_value(0, entry_double(value));
    }
    return result;
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

IfcUtil::IfcBaseClass* structural_add_structural_boundary_condition(
    IfcParse::IfcFile* file,
    const char* name,
    bool has_name,
    IfcUtil::IfcBaseClass* connection,
    const std::string& ifc_class)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("Invalid arguments");
        return nullptr;
    }
    try {
        std::string boundary_class = ifc_class;
        if (connection) {
            auto* related_connection = connection;
            if (connection->declaration().is("IfcRelConnectsStructuralMember")) {
                related_connection = ifcapi::detail::read_ref_attr(connection, "RelatedStructuralConnection");
            }
            if (!related_connection) {
                throw std::runtime_error("RelatedStructuralConnection is null or missing");
            }
            if (related_connection->declaration().is("IfcStructuralPointConnection")) {
                boundary_class = "IfcBoundaryNodeCondition";
            } else if (related_connection->declaration().is("IfcStructuralCurveConnection")) {
                boundary_class = "IfcBoundaryEdgeCondition";
            } else if (related_connection->declaration().is("IfcStructuralSurfaceConnection")) {
                boundary_class = "IfcBoundaryFaceCondition";
            } else {
                throw std::runtime_error("Unsupported structural connection type");
            }
        }
        auto* condition = file->create(file->schema()->declaration_by_name(boundary_class));
        if (has_name) {
            ifcapi::detail::write_string_attr(condition, "Name", name ? name : "");
        }
        if (connection) {
            ifcapi::detail::write_ref_attr(connection, "AppliedCondition", condition);
        }
        return condition;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void structural_edit_structural_connection_cs(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_item,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    ifcopenshell_clear_error();
    if (!file || !structural_item) {
        set_error("Invalid arguments");
        return;
    }
    try {
        if (!ifcapi::detail::entity_has_attr(structural_item, "ConditionCoordinateSystem")) {
            throw std::runtime_error("Structural item has no ConditionCoordinateSystem attribute");
        }
        auto* ccs = ifcapi::detail::read_ref_attr(structural_item, "ConditionCoordinateSystem");
        if (!ccs) {
            ccs = create_condition_coordinate_system(file);
            ifcapi::detail::write_ref_attr(structural_item, "ConditionCoordinateSystem", ccs);
        }
        if (auto* current_axis = ifcapi::detail::read_ref_attr(ccs, "Axis")) {
            if (total_inverses(file, current_axis) == 1) file->removeEntity(current_axis);
        }
        ifcapi::detail::write_ref_attr(ccs, "Axis", create_direction(file, axis));
        if (auto* current_ref_direction = ifcapi::detail::read_ref_attr(ccs, "RefDirection")) {
            if (total_inverses(file, current_ref_direction) == 1) file->removeEntity(current_ref_direction);
        }
        ifcapi::detail::write_ref_attr(ccs, "RefDirection", create_direction(file, ref_direction));
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void structural_edit_structural_item_axis(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_item,
    const std::vector<double>& axis)
{
    ifcopenshell_clear_error();
    if (!file || !structural_item) {
        set_error("Invalid arguments");
        return;
    }
    try {
        if (!ifcapi::detail::entity_has_attr(structural_item, "Axis")) {
            throw std::runtime_error("Structural item has no Axis attribute");
        }
        if (auto* current_axis = ifcapi::detail::read_ref_attr(structural_item, "Axis")) {
            if (total_inverses(file, current_axis) == 1) file->removeEntity(current_axis);
        }
        ifcapi::detail::write_ref_attr(structural_item, "Axis", create_direction(file, axis));
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void structural_edit_structural_boundary_condition(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* condition,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    try {
        if (!file || !condition) {
            throw std::runtime_error("structural_edit_structural_boundary_condition requires a file and condition");
        }
        if (!attributes) return;
        for (const auto& entry : attributes->entries) {
            if (entry.kind != ifcapi_pset::Kind::DICT || !entry.nested) {
                throw std::runtime_error("Structural boundary condition attributes must be dictionaries");
            }
            auto* type_entry = nested_entry(entry, "type");
            auto* value_entry = nested_entry(entry, "value");
            if (!type_entry || !value_entry) {
                throw std::runtime_error("Structural boundary condition attribute requires type and value");
            }
            auto type = entry_string(*type_entry);
            if (type == "string" || type == "null") {
                ifcapi_pset::Entry direct = *value_entry;
                direct.key = entry.key;
                ifcapi::detail::apply_attribute_prop(condition, direct);
            } else {
                ifcapi::detail::write_ref_attr(condition, entry.key.c_str(), create_typed_value(file, type, *value_entry));
            }
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
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

void structural_remove_structural_boundary_condition(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* connection,
    IfcUtil::IfcBaseClass* boundary_condition)
{
    ifcopenshell_clear_error();
    if (!file) return;
    try {
        if (connection) {
            auto* applied_condition = ifcapi::detail::read_ref_attr(connection, "AppliedCondition");
            if (!applied_condition) return;
            if (total_inverses(file, applied_condition) == 1) {
                file->removeEntity(applied_condition);
            }
            ifcapi::detail::write_ref_attr(connection, "AppliedCondition", nullptr);
            return;
        }
        if (!boundary_condition) {
            set_error("Either connection or boundary_condition must be provided.");
            return;
        }
        std::vector<IfcUtil::IfcBaseClass*> inverses;
        if (boundary_condition->id()) {
            auto inverse_list = file->getInverse(boundary_condition->id(), nullptr, -1);
            if (inverse_list) {
                for (auto* inverse : *inverse_list) inverses.push_back(inverse);
            }
        }
        for (auto* inverse : inverses) {
            ifcapi::detail::write_ref_attr(inverse, "AppliedCondition", nullptr);
        }
        file->removeEntity(boundary_condition);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void structural_remove_structural_connection_condition(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relation)
{
    ifcopenshell_clear_error();
    if (!file || !relation) {
        set_error("Invalid arguments");
        return;
    }
    try {
        if (ifcapi::detail::read_ref_attr(relation, "AppliedCondition")) {
            structural_remove_structural_boundary_condition(
                file,
                ifcapi::detail::read_ref_attr(relation, "RelatedStructuralConnection"),
                nullptr);
        }
        auto* history = ifcapi::detail::read_ref_attr(relation, "OwnerHistory");
        file->removeEntity(relation);
        if (history) ifcapi::bindings::entity_remove_deep2(history);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
