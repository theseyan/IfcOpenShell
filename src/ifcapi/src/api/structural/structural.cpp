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

const char* global_or_local_name(StructuralGlobalOrLocal value) {
    switch (value) {
    case StructuralGlobalOrLocal::GLOBAL_COORDS: return "GLOBAL_COORDS";
    case StructuralGlobalOrLocal::LOCAL_COORDS: return "LOCAL_COORDS";
    }
    throw std::invalid_argument("Unsupported structural coordinate system");
}

express::Base create_root_entity(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const std::string& predefined_type = {},
    const std::string& name = {},
    std::optional<express::Base> owner_history = std::nullopt)
{
    RootCreateEntityOptions options;
    options.ifc_class = ifc_class;
    if (!predefined_type.empty()) {
        options.predefined_type = predefined_type;
    }
    if (!name.empty()) {
        options.name = name;
    }
    if (owner_history) {
        options.owner_history = *owner_history;
    }
    return root_create_entity(file, options);
}

size_t total_inverses(ifcopenshell::file* file, express::Base entity) {
    if (!file || !entity || entity.id() <= 0) return 0;
    return file->instances_by_reference(static_cast<int>(entity.id())).size();
}

express::Base create_direction(ifcopenshell::file* file, const std::vector<double>& ratios) {
    if (ratios.size() != 3) {
        throw std::runtime_error("Expected a 3D direction vector");
    }
    auto direction = file->create(file->schema()->declaration_by_name("IfcDirection"));
    ifcapi::detail::write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

express::Base create_condition_coordinate_system(ifcopenshell::file* file) {
    auto point = file->create(file->schema()->declaration_by_name("IfcCartesianPoint"));
    ifcapi::detail::write_double_aggregate(point, "Coordinates", {0.0, 0.0, 0.0});
    auto placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    ifcapi::detail::write_ref_attr(placement, "Location", point);
    return placement;
}

const ifcapi_pset::Entry* nested_entry(const ifcapi_pset::Entry& entry, const std::string& key) {
    if (!entry.nested) return {};
    for (const auto& nested : entry.nested->entries) {
        if (nested.key == key) return &nested;
    }
    return {};
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

express::Base create_typed_value(
    ifcopenshell::file* file,
    const std::string& ifc_type,
    const ifcapi_pset::Entry& value)
{
    auto* declaration = file->schema()->declaration_by_name(ifc_type);
    auto* type_declaration = declaration ? declaration->as_type_declaration() : nullptr;
    if (!type_declaration) {
        throw std::runtime_error("Unable to create " + ifc_type);
    }
    auto result = file->create(type_declaration);
    if (ifc_type == "IfcBoolean") {
        if (value.kind == ifcapi_pset::Kind::BOOL || value.kind == ifcapi_pset::Kind::TYPED_BOOL) {
            result.set_attribute_value(0, value.b_val);
        } else {
            throw std::runtime_error("IfcBoolean requires a boolean value");
        }
    } else if (value.kind == ifcapi_pset::Kind::STRING || value.kind == ifcapi_pset::Kind::TYPED_STRING) {
        result.set_attribute_value(0, value.s_val);
    } else {
        result.set_attribute_value(0, entry_double(value));
    }
    return result;
}

} // namespace

express::Base structural_add_structural_analysis_model(
    ifcopenshell::file* file,
    std::optional<express::Base> owner_history)
{
    ifcopenshell_clear_error();
    return create_root_entity(file, "IfcStructuralAnalysisModel", "LOADING_3D", {}, owner_history);
}

express::Base structural_assign_structural_analysis_model(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* structural_analysis_model,
    const StructuralAssignStructuralAnalysisModelOptions& options)
{
    ifcopenshell_clear_error();
    GroupAssignGroupOptions opts;
    opts.products = products;
    opts.group = ifcapi::detail::deref_or_empty(structural_analysis_model);
    if (options.owner_history) opts.owner_history = *options.owner_history;
    if (options.user) opts.user = *options.user;
    if (options.application) opts.application = *options.application;
    return group_assign_group(file, opts);
}

void structural_unassign_structural_analysis_model(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* structural_analysis_model,
    const StructuralUnassignStructuralAnalysisModelOptions& options)
{
    ifcopenshell_clear_error();
    GroupUnassignGroupOptions opts;
    opts.products = products;
    opts.group = ifcapi::detail::deref_or_empty(structural_analysis_model);
    if (options.user) opts.user = *options.user;
    if (options.application) opts.application = *options.application;
    group_unassign_group(file, opts);
}

express::Base structural_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    std::optional<express::Base> owner_history)
{
    ifcopenshell_clear_error();
    auto relating_product_ref = ifcapi::detail::deref_or_empty(relating_product);
    auto related_object_ref = ifcapi::detail::deref_or_empty(related_object);
    if (!file || !relating_product_ref || !related_object_ref) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        for (auto rel : ifcapi::detail::read_inverse_aggregate(relating_product_ref, "ReferencedBy")) {
            if (!rel || !rel.declaration().is("IfcRelAssignsToProduct")) {
                continue;
            }
            auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            if (ifcapi::detail::contains_ref(related_objects, related_object_ref)) {
                return rel;
            }
            related_objects.push_back(related_object_ref);
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            return rel;
        }

        auto rel = create_root_entity(file, "IfcRelAssignsToProduct", {}, {}, owner_history);
        if (!rel) {
            return {};
        }
        ifcapi::detail::write_ref_attr(rel, "RelatingProduct", relating_product_ref);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {related_object_ref});
        return rel;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

express::Base structural_add_structural_activity(
    ifcopenshell::file* file,
    express::Base* applied_load,
    express::Base* structural_member,
    const std::string& ifc_class,
    const std::string& predefined_type,
    StructuralGlobalOrLocal global_or_local,
    const StructuralAddStructuralActivityOptions& options)
{
    ifcopenshell_clear_error();
    auto applied_load_ref = ifcapi::detail::deref_or_empty(applied_load);
    auto structural_member_ref = ifcapi::detail::deref_or_empty(structural_member);
    if (!file || !applied_load_ref || !structural_member_ref) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const char* coordinate_system = global_or_local_name(global_or_local);
        auto activity = create_root_entity(file, ifc_class, predefined_type, {}, options.activity_owner_history);
        if (!activity) {
            return {};
        }
        ifcapi::detail::write_ref_attr(activity, "AppliedLoad", applied_load_ref);
        ifcapi::detail::write_enum_attr(activity, "GlobalOrLocal", coordinate_system);

        auto rel = create_root_entity(file, "IfcRelConnectsStructuralActivity", {}, {}, options.relationship_owner_history);
        if (!rel) {
            return {};
        }
        ifcapi::detail::write_ref_attr(rel, "RelatingElement", structural_member_ref);
        ifcapi::detail::write_ref_attr(rel, "RelatedStructuralActivity", activity);
        return activity;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

express::Base structural_add_structural_load(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    std::optional<std::string> name)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        auto load = file->create(file->schema()->declaration_by_name(ifc_class));
        if (name) {
            ifcapi::detail::write_string_attr(load, "Name", *name);
        }
        return load;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

express::Base structural_add_structural_load_case(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    std::optional<express::Base> owner_history)
{
    ifcopenshell_clear_error();
    auto load_case = create_root_entity(file, "IfcStructuralLoadCase", "LOAD_CASE", name, owner_history);
    if (load_case) {
        ifcapi::detail::write_enum_attr(load_case, "ActionType", action_type);
        ifcapi::detail::write_enum_attr(load_case, "ActionSource", action_source);
    }
    return load_case;
}

express::Base structural_add_structural_load_group(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    std::optional<express::Base> owner_history)
{
    ifcopenshell_clear_error();
    auto load_group = create_root_entity(file, "IfcStructuralLoadGroup", "LOAD_GROUP", name, owner_history);
    if (load_group) {
        ifcapi::detail::write_enum_attr(load_group, "ActionType", action_type);
        ifcapi::detail::write_enum_attr(load_group, "ActionSource", action_source);
    }
    return load_group;
}

express::Base structural_add_structural_member_connection(
    ifcopenshell::file* file,
    express::Base* relating_structural_member,
    express::Base* related_structural_connection,
    std::optional<express::Base> owner_history)
{
    ifcopenshell_clear_error();
    auto relating_structural_member_ref = ifcapi::detail::deref_or_empty(relating_structural_member);
    auto related_structural_connection_ref = ifcapi::detail::deref_or_empty(related_structural_connection);
    if (!file || !relating_structural_member_ref || !related_structural_connection_ref) {
        set_error("Invalid arguments");
        return {};
    }
    for (auto rel : ifcapi::detail::read_inverse_aggregate(
             related_structural_connection_ref,
             "ConnectsStructuralMembers")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatingStructuralMember") == relating_structural_member_ref) {
            return rel;
        }
    }
    auto rel = create_root_entity(file, "IfcRelConnectsStructuralMember", {}, {}, owner_history);
    if (!rel) {
        return {};
    }
    ifcapi::detail::write_ref_attr(rel, "RelatingStructuralMember", relating_structural_member_ref);
    ifcapi::detail::write_ref_attr(rel, "RelatedStructuralConnection", related_structural_connection_ref);
    return rel;
}

express::Base structural_add_structural_boundary_condition(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const StructuralAddStructuralBoundaryConditionOptions& options)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        std::string boundary_class = ifc_class;
        auto connection_ref = options.connection.value_or(express::Base());
        if (connection_ref) {
            auto related_connection = connection_ref;
            if (connection_ref.declaration().is("IfcRelConnectsStructuralMember")) {
                related_connection = ifcapi::detail::read_ref_attr(connection_ref, "RelatedStructuralConnection");
            }
            if (!related_connection) {
                throw std::runtime_error("RelatedStructuralConnection is null or missing");
            }
            if (related_connection.declaration().is("IfcStructuralPointConnection")) {
                boundary_class = "IfcBoundaryNodeCondition";
            } else if (related_connection.declaration().is("IfcStructuralCurveConnection")) {
                boundary_class = "IfcBoundaryEdgeCondition";
            } else if (related_connection.declaration().is("IfcStructuralSurfaceConnection")) {
                boundary_class = "IfcBoundaryFaceCondition";
            } else {
                throw std::runtime_error("Unsupported structural connection type");
            }
        }
        auto condition = file->create(file->schema()->declaration_by_name(boundary_class));
        if (options.name) {
            ifcapi::detail::write_string_attr(condition, "Name", *options.name);
        }
        if (connection_ref) {
            ifcapi::detail::write_ref_attr(connection_ref, "AppliedCondition", condition);
        }
        return condition;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    }
}

void structural_edit_structural_connection_cs(
    ifcopenshell::file* file,
    express::Base* structural_item,
    const Vec3& axis,
    const Vec3& ref_direction)
{
    ifcopenshell_clear_error();
    auto structural_item_ref = ifcapi::detail::deref_or_empty(structural_item);
    if (!file || !structural_item_ref) {
        set_error("Invalid arguments");
        return;
    }
    try {
        if (!ifcapi::detail::entity_has_attr(structural_item_ref, "ConditionCoordinateSystem")) {
            throw std::runtime_error("Structural item has no ConditionCoordinateSystem attribute");
        }
        auto ccs = ifcapi::detail::read_ref_attr(structural_item_ref, "ConditionCoordinateSystem");
        if (!ccs) {
            ccs = create_condition_coordinate_system(file);
            ifcapi::detail::write_ref_attr(structural_item_ref, "ConditionCoordinateSystem", ccs);
        }
        if (auto current_axis = ifcapi::detail::read_ref_attr(ccs, "Axis")) {
            if (total_inverses(file, current_axis) == 1) file->remove_entity(current_axis);
        }
        ifcapi::detail::write_ref_attr(ccs, "Axis", create_direction(file, std::vector<double>(axis.begin(), axis.end())));
        if (auto current_ref_direction = ifcapi::detail::read_ref_attr(ccs, "RefDirection")) {
            if (total_inverses(file, current_ref_direction) == 1) file->remove_entity(current_ref_direction);
        }
        ifcapi::detail::write_ref_attr(
            ccs, "RefDirection", create_direction(file, std::vector<double>(ref_direction.begin(), ref_direction.end())));
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void structural_edit_structural_item_axis(
    ifcopenshell::file* file,
    express::Base* structural_item,
    const Vec3& axis)
{
    ifcopenshell_clear_error();
    auto structural_item_ref = ifcapi::detail::deref_or_empty(structural_item);
    if (!file || !structural_item_ref) {
        set_error("Invalid arguments");
        return;
    }
    try {
        if (!ifcapi::detail::entity_has_attr(structural_item_ref, "Axis")) {
            throw std::runtime_error("Structural item has no Axis attribute");
        }
        if (auto current_axis = ifcapi::detail::read_ref_attr(structural_item_ref, "Axis")) {
            if (total_inverses(file, current_axis) == 1) file->remove_entity(current_axis);
        }
        ifcapi::detail::write_ref_attr(
            structural_item_ref, "Axis", create_direction(file, std::vector<double>(axis.begin(), axis.end())));
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void structural_edit_structural_boundary_condition(
    ifcopenshell::file* file,
    express::Base* condition,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    try {
        auto condition_ref = ifcapi::detail::deref_or_empty(condition);
        if (!file || !condition_ref) {
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
                ifcapi::detail::apply_attribute_prop(condition_ref, direct);
            } else {
                ifcapi::detail::write_ref_attr(condition_ref, entry.key.c_str(), create_typed_value(file, type, *value_entry));
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

express::Base structural_assign_to_building(
    ifcopenshell::file* file,
    express::Base* structural_analysis_model,
    express::Base* building,
    std::optional<express::Base> owner_history)
{
    ifcopenshell_clear_error();
    auto structural_analysis_model_ref = ifcapi::detail::deref_or_empty(structural_analysis_model);
    auto building_ref = ifcapi::detail::deref_or_empty(building);
    if (!file || !structural_analysis_model_ref || !building_ref) {
        set_error("Invalid arguments");
        return {};
    }
    for (auto rel : ifcapi::detail::read_inverse_aggregate(structural_analysis_model_ref, "ServicesBuildings")) {
        auto buildings = ifcapi::detail::read_ref_aggregate(rel, "RelatedBuildings");
        if (ifcapi::detail::contains_ref(buildings, building_ref)) {
            return rel;
        }
        buildings.push_back(building_ref);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedBuildings", buildings);
        return rel;
    }
    auto rel = create_root_entity(file, "IfcRelServicesBuildings", {}, {}, owner_history);
    if (!rel) {
        return {};
    }
    ifcapi::detail::write_ref_attr(rel, "RelatingSystem", structural_analysis_model_ref);
    ifcapi::detail::write_ref_aggregate(rel, "RelatedBuildings", {building_ref});
    return rel;
}

void structural_remove_structural_analysis_model(
    ifcopenshell::file* file,
    express::Base* structural_analysis_model)
{
    ifcopenshell_clear_error();
    auto structural_analysis_model_ref = ifcapi::detail::deref_or_empty(structural_analysis_model);
    if (!file || !structural_analysis_model_ref) return;
    for (auto rel : ifcapi::detail::read_inverse_aggregate(structural_analysis_model_ref, "IsGroupedBy")) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    ifcapi::detail::remove_with_history(file, structural_analysis_model_ref);
}

void structural_remove_structural_load(ifcopenshell::file* file, express::Base* structural_load) {
    ifcopenshell_clear_error();
    auto structural_load_ref = ifcapi::detail::deref_or_empty(structural_load);
    if (file && structural_load_ref) {
        file->remove_entity(structural_load_ref);
    }
}

void structural_remove_structural_load_case(ifcopenshell::file* file, express::Base* structural_load_case) {
    ifcopenshell_clear_error();
    auto structural_load_case_ref = ifcapi::detail::deref_or_empty(structural_load_case);
    if (!file || !structural_load_case_ref) return;
    for (auto rel : ifcapi::detail::read_inverse_aggregate(structural_load_case_ref, "IsGroupedBy")) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    ifcapi::detail::remove_with_history(file, structural_load_case_ref);
}

void structural_remove_structural_load_group(ifcopenshell::file* file, express::Base* structural_load_group) {
    ifcopenshell_clear_error();
    auto structural_load_group_ref = ifcapi::detail::deref_or_empty(structural_load_group);
    if (!file || !structural_load_group_ref) return;
    if (structural_load_group_ref.id() > 0) {
        std::vector<express::Base> rels;
        for (auto inverse : file->instances_by_reference(static_cast<int>(structural_load_group_ref.id()))) {
            if (inverse && inverse.declaration().is("IfcRelAssignsToGroup")
                && ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() == 1) {
                rels.push_back(inverse);
            }
        }
        for (auto rel : rels) {
            ifcapi::detail::remove_with_history(file, rel);
        }
    }
    ifcapi::detail::remove_with_history(file, structural_load_group_ref);
}

void structural_remove_structural_boundary_condition(
    ifcopenshell::file* file,
    const StructuralRemoveStructuralBoundaryConditionOptions& options)
{
    ifcopenshell_clear_error();
    if (!file) return;
    try {
        auto connection_ref = options.connection.value_or(express::Base());
        auto boundary_condition_ref = options.boundary_condition.value_or(express::Base());
        if (connection_ref) {
            auto applied_condition = ifcapi::detail::read_ref_attr(connection_ref, "AppliedCondition");
            if (!applied_condition) return;
            if (total_inverses(file, applied_condition) == 1) {
                file->remove_entity(applied_condition);
            }
            ifcapi::detail::write_ref_attr(connection_ref, "AppliedCondition", {});
            return;
        }
        if (!boundary_condition_ref) {
            set_error("Either connection or boundary_condition must be provided.");
            return;
        }
        std::vector<express::Base> inverses;
        if (boundary_condition_ref.id() > 0) {
            for (auto inverse : file->instances_by_reference(static_cast<int>(boundary_condition_ref.id()))) {
                inverses.push_back(inverse);
            }
        }
        for (auto inverse : inverses) {
            ifcapi::detail::write_ref_attr(inverse, "AppliedCondition", {});
        }
        file->remove_entity(boundary_condition_ref);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void structural_remove_structural_connection_condition(
    ifcopenshell::file* file,
    express::Base* relation)
{
    ifcopenshell_clear_error();
    auto relation_ref = ifcapi::detail::deref_or_empty(relation);
    if (!file || !relation_ref) {
        set_error("Invalid arguments");
        return;
    }
    try {
        if (ifcapi::detail::read_ref_attr(relation_ref, "AppliedCondition")) {
            auto related_connection = ifcapi::detail::read_ref_attr(relation_ref, "RelatedStructuralConnection");
            StructuralRemoveStructuralBoundaryConditionOptions rm_opts;
            if (related_connection) rm_opts.connection = related_connection;
            structural_remove_structural_boundary_condition(file, rm_opts);
        }
        auto history = ifcapi::detail::read_ref_attr(relation_ref, "OwnerHistory");
        file->remove_entity(relation_ref);
        if (history) ifcapi::bindings::entity_remove_deep(&history);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void structural_edit_structural_analysis_model(
    ifcopenshell::file* file,
    express::Base analysis_model,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, analysis_model, attributes);
}

void structural_edit_structural_load(
    ifcopenshell::file* file,
    express::Base load,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, load, attributes);
}

void structural_edit_structural_load_case(
    ifcopenshell::file* file,
    express::Base load_case,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, load_case, attributes);
}

} // namespace bindings
} // namespace ifcapi
