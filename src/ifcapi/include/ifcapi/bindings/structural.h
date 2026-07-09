// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_STRUCTURAL_H
#define IFCAPI_BINDINGS_STRUCTURAL_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

struct StructuralAssignStructuralAnalysisModelOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct StructuralUnassignStructuralAnalysisModelOptions {
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct StructuralAddStructuralActivityOptions {
    std::optional<express::Base> activity_owner_history;
    std::optional<express::Base> relationship_owner_history;
};

struct StructuralAddStructuralBoundaryConditionOptions {
    std::optional<std::string> name;
    std::optional<express::Base> connection;
};

struct StructuralRemoveStructuralBoundaryConditionOptions {
    std::optional<express::Base> connection;
    std::optional<express::Base> boundary_condition;
};

IFCAPI_BINDING express::Base structural_add_structural_analysis_model(
    ifcopenshell::file* file,
    std::optional<express::Base> owner_history);
IFCAPI_BINDING express::Base structural_assign_structural_analysis_model(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* structural_analysis_model,
    const StructuralAssignStructuralAnalysisModelOptions& options);
IFCAPI_BINDING void structural_unassign_structural_analysis_model(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* structural_analysis_model,
    const StructuralUnassignStructuralAnalysisModelOptions& options);
IFCAPI_BINDING express::Base structural_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    std::optional<express::Base> owner_history);
IFCAPI_BINDING express::Base structural_add_structural_activity(
    ifcopenshell::file* file,
    express::Base* applied_load,
    express::Base* structural_member,
    const std::string& ifc_class,
    const std::string& predefined_type,
    const std::string& global_or_local,
    const StructuralAddStructuralActivityOptions& options);
IFCAPI_BINDING express::Base structural_add_structural_load(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    std::optional<std::string> name);
IFCAPI_BINDING express::Base structural_add_structural_load_case(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    std::optional<express::Base> owner_history);
IFCAPI_BINDING express::Base structural_add_structural_load_group(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    std::optional<express::Base> owner_history);
IFCAPI_BINDING express::Base structural_add_structural_member_connection(
    ifcopenshell::file* file,
    express::Base* relating_structural_member,
    express::Base* related_structural_connection,
    std::optional<express::Base> owner_history);
IFCAPI_BINDING express::Base structural_add_structural_boundary_condition(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const StructuralAddStructuralBoundaryConditionOptions& options);
IFCAPI_BINDING void structural_edit_structural_connection_cs(
    ifcopenshell::file* file,
    express::Base* structural_item,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction);
IFCAPI_BINDING void structural_edit_structural_item_axis(
    ifcopenshell::file* file,
    express::Base* structural_item,
    const std::vector<double>& axis);
IFCAPI_BINDING void structural_edit_structural_boundary_condition(
    ifcopenshell::file* file,
    express::Base* condition,
    ifcopenshell_pset_props_t* attributes);
IFCAPI_BINDING express::Base structural_assign_to_building(
    ifcopenshell::file* file,
    express::Base* structural_analysis_model,
    express::Base* building,
    std::optional<express::Base> owner_history);
IFCAPI_BINDING void structural_remove_structural_analysis_model(
    ifcopenshell::file* file,
    express::Base* structural_analysis_model);
IFCAPI_BINDING void structural_remove_structural_load(
    ifcopenshell::file* file,
    express::Base* structural_load);
IFCAPI_BINDING void structural_remove_structural_load_case(
    ifcopenshell::file* file,
    express::Base* structural_load_case);
IFCAPI_BINDING void structural_remove_structural_load_group(
    ifcopenshell::file* file,
    express::Base* structural_load_group);
IFCAPI_BINDING void structural_remove_structural_boundary_condition(
    ifcopenshell::file* file,
    const StructuralRemoveStructuralBoundaryConditionOptions& options);
IFCAPI_BINDING void structural_remove_structural_connection_condition(
    ifcopenshell::file* file,
    express::Base* relation);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STRUCTURAL_H */
