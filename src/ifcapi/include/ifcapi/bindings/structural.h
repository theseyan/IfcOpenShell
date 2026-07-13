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

/// Options for assigning products to a structural analysis model.
struct StructuralAssignStructuralAnalysisModelOptions {
    /// Owner history applied to the IfcRelAssignsToGroup relationship. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/// Options for removing products from a structural analysis model.
struct StructuralUnassignStructuralAnalysisModelOptions {
    /// IfcPersonAndOrganization used to update the relationship OwnerHistory.
    std::optional<express::Base> user;
    /// IfcApplication used to update the relationship OwnerHistory.
    std::optional<express::Base> application;
};

/// Ownership options for creating a structural activity and its relationship.
struct StructuralAddStructuralActivityOptions {
    /// Owner history for the IfcStructuralActivity entity. When omitted, no OwnerHistory is set.
    std::optional<express::Base> activity_owner_history;
    /// Owner history for the IfcRelConnectsStructuralActivity relationship. When omitted, no OwnerHistory is set.
    std::optional<express::Base> relationship_owner_history;
};

/// Options for creating and applying a structural boundary condition.
struct StructuralAddStructuralBoundaryConditionOptions {
    /// Name for the boundary condition entity.
    std::optional<std::string> name;
    /// IfcRelConnectsStructuralMember or IfcStructuralConnection to which the condition is applied. When provided, the boundary class is inferred from the connection type.
    std::optional<express::Base> connection;
};

/// Options for removing a structural boundary condition.
struct StructuralRemoveStructuralBoundaryConditionOptions {
    /// IfcStructuralConnection whose AppliedCondition to clear and delete. Takes precedence over boundary_condition.
    std::optional<express::Base> connection;
    /// IfcBoundaryCondition entity to remove. Used when connection is omitted.
    std::optional<express::Base> boundary_condition;
};

/**
 * Create an IfcStructuralAnalysisModel with PredefinedType LOADING_3D.
 *
 * @param file File that receives the new entity.
 * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
 * @return Newly created IfcStructuralAnalysisModel.
 */
IFCAPI_BINDING express::Base structural_add_structural_analysis_model(
    ifcopenshell::file* file,
    std::optional<express::Base> owner_history);
/**
 * Assign products to an IfcStructuralAnalysisModel via IfcRelAssignsToGroup.
 *
 * @param file File containing the products and model.
 * @param products Products to assign to the model.
 * @param structural_analysis_model IfcStructuralAnalysisModel to assign to.
 * @param options Ownership options for the relationship.
 * @return The IfcRelAssignsToGroup relationship.
 */
IFCAPI_BINDING express::Base structural_assign_structural_analysis_model(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* structural_analysis_model,
    const StructuralAssignStructuralAnalysisModelOptions& options);
/**
 * Remove products from an IfcStructuralAnalysisModel's group.
 *
 * @param file File containing the products and model.
 * @param products Products to unassign from the model.
 * @param structural_analysis_model IfcStructuralAnalysisModel to unassign from.
 * @param options Ownership options for updating the relationship.
 */
IFCAPI_BINDING void structural_unassign_structural_analysis_model(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* structural_analysis_model,
    const StructuralUnassignStructuralAnalysisModelOptions& options);
/**
 * Assign a structural member or connection to a product via IfcRelAssignsToProduct.
 *
 * If a relationship already exists between the two, the object is appended
 * to the existing relationship's RelatedObjects. Otherwise a new
 * IfcRelAssignsToProduct is created.
 *
 * @param file File containing both entities.
 * @param relating_product IfcProduct that the structural item references.
 * @param related_object Structural item to assign.
 * @param owner_history Owner history for new relationships. When omitted, no owner history is assigned.
 * @return The IfcRelAssignsToProduct relationship.
 */
IFCAPI_BINDING express::Base structural_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    std::optional<express::Base> owner_history);
/**
 * Create an IfcStructuralActivity and connect it to a structural member.
 *
 * Creates the activity entity of the given class and predefined type, sets
 * its AppliedLoad and GlobalOrLocal attributes, and creates an
 * IfcRelConnectsStructuralActivity linking the activity to the structural
 * member.
 *
 * @param file File that receives the new entities.
 * @param applied_load IfcStructuralLoad to apply.
 * @param structural_member Structural member to connect the activity to.
 * @param ifc_class IFC class name (e.g. "IfcStructuralPlanarAction").
 * @param predefined_type Predefined type enum value.
 * @param global_or_local "GLOBAL_COORDS" or "LOCAL_COORDS".
 * @param options Ownership options for the activity and relationship.
 * @return Newly created IfcStructuralActivity.
 */
IFCAPI_BINDING express::Base structural_add_structural_activity(
    ifcopenshell::file* file,
    express::Base* applied_load,
    express::Base* structural_member,
    const std::string& ifc_class,
    const std::string& predefined_type,
    const std::string& global_or_local,
    const StructuralAddStructuralActivityOptions& options);
/**
 * Create an IfcStructuralLoad subclass entity.
 *
 * @param file File that receives the new entity.
 * @param ifc_class IFC class name (e.g. "IfcStructuralLoadConfiguration").
 * @param name Optional name for the load entity.
 * @return Newly created IfcStructuralLoad subclass.
 */
IFCAPI_BINDING express::Base structural_add_structural_load(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    std::optional<std::string> name);
/**
 * Create an IfcStructuralLoadCase entity.
 *
 * Creates the load case with PredefinedType LOAD_CASE and the given
 * ActionType and ActionSource enum values.
 *
 * @param file File that receives the new entity.
 * @param name Name for the load case.
 * @param action_type ActionType enum value (e.g. "PERMANENT_G", "VARIABLE_Q").
 * @param action_source ActionSource enum value (e.g. "WIND", "IMPOSED").
 * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
 * @return Newly created IfcStructuralLoadCase.
 */
IFCAPI_BINDING express::Base structural_add_structural_load_case(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    std::optional<express::Base> owner_history);
/**
 * Create an IfcStructuralLoadGroup entity.
 *
 * Creates the load group with PredefinedType LOAD_GROUP and the given
 * ActionType and ActionSource enum values.
 *
 * @param file File that receives the new entity.
 * @param name Name for the load group.
 * @param action_type ActionType enum value.
 * @param action_source ActionSource enum value.
 * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
 * @return Newly created IfcStructuralLoadGroup.
 */
IFCAPI_BINDING express::Base structural_add_structural_load_group(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    std::optional<express::Base> owner_history);
/**
 * Connect a structural member to a structural connection via IfcRelConnectsStructuralMember.
 *
 * If a connection already exists between the two, the existing relationship
 * is returned.
 *
 * @param file File containing both entities.
 * @param relating_structural_member IfcStructuralMember to connect.
 * @param related_structural_connection IfcStructuralConnection to connect to.
 * @param owner_history Owner history for the new relationship. When omitted, no owner history is assigned.
 * @return The IfcRelConnectsStructuralMember relationship.
 */
IFCAPI_BINDING express::Base structural_add_structural_member_connection(
    ifcopenshell::file* file,
    express::Base* relating_structural_member,
    express::Base* related_structural_connection,
    std::optional<express::Base> owner_history);
/**
 * Create an IfcBoundaryCondition subclass entity.
 *
 * When a connection is provided, the boundary class is inferred:
 * IfcStructuralPointConnection yields IfcBoundaryNodeCondition,
 * IfcStructuralCurveConnection yields IfcBoundaryEdgeCondition,
 * IfcStructuralSurfaceConnection yields IfcBoundaryFaceCondition.
 * The condition is applied to the connection's AppliedCondition attribute.
 *
 * @param file File that receives the new entity.
 * @param ifc_class IFC boundary condition class name. Overridden when connection is provided.
 * @param options Name and connection options.
 * @return Newly created IfcBoundaryCondition subclass.
 */
IFCAPI_BINDING express::Base structural_add_structural_boundary_condition(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const StructuralAddStructuralBoundaryConditionOptions& options);
/**
 * Edit the ConditionCoordinateSystem of a structural item.
 *
 * Replaces the Axis and RefDirection directions on the item's
 * ConditionCoordinateSystem (IfcAxis2Placement3D). Creates the placement
 * if absent. Old direction entities are deleted if unreferenced.
 *
 * @param file File containing the structural item.
 * @param structural_item Structural item (e.g. IfcStructuralPointConnection).
 * @param axis 3-element direction ratios for the Axis attribute.
 * @param ref_direction 3-element direction ratios for the RefDirection attribute.
 */
IFCAPI_BINDING void structural_edit_structural_connection_cs(
    ifcopenshell::file* file,
    express::Base* structural_item,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction);
/**
 * Edit the Axis direction of a structural item.
 *
 * Replaces the Axis direction on the structural item (e.g.
 * IfcStructuralCurveMember). The old direction entity is deleted if
 * unreferenced.
 *
 * @param file File containing the structural item.
 * @param structural_item Structural item with an Axis attribute.
 * @param axis 3-element direction ratios.
 */
IFCAPI_BINDING void structural_edit_structural_item_axis(
    ifcopenshell::file* file,
    express::Base* structural_item,
    const std::vector<double>& axis);
/**
 * Edit attributes of an IfcBoundaryCondition subclass.
 *
 * Each entry in the attributes mapping must contain "type" and
 * "value" sub-entries. The type specifies the IFC typed value class (e.g.
 * "IfcBoolean", "IfcForceMeasure") or "string"/"blank" for direct values.
 *
 * @param file File containing the boundary condition.
 * @param condition IfcBoundaryCondition entity to edit.
 * @param attributes Property bag of typed attribute entries.
 */
IFCAPI_BINDING void structural_edit_structural_boundary_condition(
    ifcopenshell::file* file,
    express::Base* condition,
    ifcopenshell_pset_props_t* attributes);
/**
 * Assign an IfcStructuralAnalysisModel to a building via IfcRelServicesBuildings.
 *
 * If a relationship already exists for this model, the building is appended
 * to its RelatedBuildings aggregate.
 *
 * @param file File containing both entities.
 * @param structural_analysis_model IfcStructuralAnalysisModel to assign.
 * @param building IfcBuilding to assign to.
 * @param owner_history Owner history for the new relationship. When omitted, no owner history is assigned.
 * @return The IfcRelServicesBuildings relationship.
 */
IFCAPI_BINDING express::Base structural_assign_to_building(
    ifcopenshell::file* file,
    express::Base* structural_analysis_model,
    express::Base* building,
    std::optional<express::Base> owner_history);
/**
 * Remove an IfcStructuralAnalysisModel from the file.
 *
 * Removes the model's IfcRelAssignsToGroup relationships and then the
 * model entity itself.
 *
 * @param file File containing the model.
 * @param structural_analysis_model IfcStructuralAnalysisModel to remove.
 */
IFCAPI_BINDING void structural_remove_structural_analysis_model(
    ifcopenshell::file* file,
    express::Base* structural_analysis_model);
/**
 * Remove an IfcStructuralLoad subclass from the file.
 *
 * @param file File containing the load.
 * @param structural_load IfcStructuralLoad entity to remove.
 */
IFCAPI_BINDING void structural_remove_structural_load(ifcopenshell::file* file, express::Base* structural_load);
/**
 * Remove an IfcStructuralLoadCase from the file.
 *
 * Removes the load case's IfcRelAssignsToGroup relationships and then the
 * entity itself.
 *
 * @param file File containing the load case.
 * @param structural_load_case IfcStructuralLoadCase to remove.
 */
IFCAPI_BINDING void structural_remove_structural_load_case(ifcopenshell::file* file, express::Base* structural_load_case);
/**
 * Remove an IfcStructuralLoadGroup from the file.
 *
 * Removes singleton IfcRelAssignsToGroup relationships and then the entity
 * itself.
 *
 * @param file File containing the load group.
 * @param structural_load_group IfcStructuralLoadGroup to remove.
 */
IFCAPI_BINDING void structural_remove_structural_load_group(ifcopenshell::file* file, express::Base* structural_load_group);
/**
 * Remove a boundary condition from a structural connection.
 *
 * When connection is provided, clears and deletes its AppliedCondition.
 * When boundary_condition is provided instead, clears AppliedCondition on
 * all referencing entities and deletes the condition. At least one of
 * connection or boundary_condition must be provided.
 *
 * @param file File containing the entities.
 * @param options Connection or boundary condition to remove.
 */
IFCAPI_BINDING void structural_remove_structural_boundary_condition(
    ifcopenshell::file* file,
    const StructuralRemoveStructuralBoundaryConditionOptions& options);
/**
 * Remove an IfcRelConnectsStructuralMember and its AppliedCondition.
 *
 * Removes the boundary condition from the related structural connection
 * (if any), then removes the relationship and its OwnerHistory.
 *
 * @param file File containing the relationship.
 * @param relation IfcRelConnectsStructuralMember to remove.
 */
IFCAPI_BINDING void structural_remove_structural_connection_condition(
    ifcopenshell::file* file,
    express::Base* relation);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STRUCTURAL_H */
