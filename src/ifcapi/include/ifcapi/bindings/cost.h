// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COST_H
#define IFCAPI_BINDINGS_COST_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Options for editing an IfcCostValue's unit basis.
 */
struct CostEditCostValueOptions {
    /// When true, the UnitBasis (IfcMeasureWithUnit) is replaced using value_component and unit_component.
    bool edit_unit_basis = false;
    /// When true (with edit_unit_basis), the existing UnitBasis is removed rather than replaced.
    bool clear_unit_basis = false;
    /// Numeric value for the new UnitBasis measure. Used when edit_unit_basis is true and clear_unit_basis is false.
    double value_component = 0.0;
    /// IfcUnit for the new UnitBasis. Required when edit_unit_basis is true and clear_unit_basis is false.
    std::optional<express::Base> unit_component;
};

/// Options for creating an IfcCostItem.
struct CostAddCostItemOptions {
    /// IfcCostSchedule to receive the new IfcCostItem. Takes precedence over cost_item when both are provided.
    std::optional<express::Base> cost_schedule;
    /// Parent IfcCostItem under which to nest the new item. Used only when cost_schedule is omitted.
    std::optional<express::Base> cost_item;
    /// Owner history applied to created entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/// Ownership options for assigning quantities to an IfcCostItem.
struct CostAssignCostItemQuantityOptions {
    /// Owner history applied to the IfcRelAssignsToControl relationship. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/// Ownership options for removing quantities from an IfcCostItem.
struct CostUnassignCostItemQuantityOptions {
    /// IfcPersonAndOrganization used to update the relationship OwnerHistory.
    std::optional<express::Base> user;
    /// IfcApplication used to update the relationship OwnerHistory.
    std::optional<express::Base> application;
};

/// Ownership options for copying an IfcCostSchedule.
struct CostCopyCostScheduleOptions {
    /// Owner history applied to the copied schedule and its items. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/**
 * Edit attributes of an IfcCostValue.
 *
 * Applies attribute changes from the property bag. The AppliedValue attribute
 * is wrapped as IfcMonetaryMeasure. When edit_unit_basis is true, the
 * UnitBasis (IfcMeasureWithUnit) is replaced or cleared.
 *
 * @param file File containing the cost value.
 * @param cost_value IfcCostValue entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 * @param options Unit basis replacement options.
 */
IFCAPI_BINDING void cost_edit_cost_value(
    ifcopenshell::file* file,
    express::Base* cost_value,
    ifcopenshell_pset_props_t* attributes,
    const CostEditCostValueOptions& options);

/**
 * Set an IfcCostValue's attributes from a formula expression.
 *
 * Parses a text formula (e.g. "100*5", "sum(10+20)") and applies the
 * resulting value tree to the cost value's AppliedValue, Category,
 * ArithmeticOperator, and Component cost values.
 *
 * @param file File containing the cost value.
 * @param cost_value IfcCostValue entity to update.
 * @param formula Formula string to parse.
 */
IFCAPI_BINDING void cost_edit_cost_value_formula(
    ifcopenshell::file* file,
    express::Base* cost_value,
    const std::string& formula);

/**
 * Create an IfcCostSchedule entity.
 *
 * Creates the schedule with the given name, predefined type, and update date.
 * The update_date is stored as an IfcDateAndTime on IFC2X3 or as a string
 * on IFC4+.
 *
 * @param file File that receives the new entity.
 * @param name Schedule name. When omitted or empty, no name is assigned.
 * @param predefined_type IFC predefined type enum value (e.g. "BUDGET", "COSTPLAN").
 * @param update_date ISO 8601 date-time string for the UpdateDate attribute.
 * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
 * @return Newly created IfcCostSchedule.
 */
IFCAPI_BINDING express::Base cost_add_cost_schedule(
    ifcopenshell::file* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& update_date,
    std::optional<express::Base> owner_history);

/**
 * Create an IfcCostItem entity.
 *
 * When both cost_schedule and cost_item are provided, cost_schedule takes
 * precedence and the item is assigned to it via IfcRelAssignsToControl.
 * When only cost_item is provided, the item is nested under it via
 * IfcRelNests.
 *
 * @param file File that receives the new entity.
 * @param options Schedule/parent placement and ownership options.
 * @return Newly created IfcCostItem.
 */
IFCAPI_BINDING express::Base cost_add_cost_item(
    ifcopenshell::file* file,
    const CostAddCostItemOptions& options);

/**
 * Create an IfcCostValue and attach it to a parent entity.
 *
 * Appends the new IfcCostValue to the parent's CostValues (IfcCostItem),
 * BaseCosts (IfcConstructionResource), or Components (IfcCostValue)
 * aggregate, depending on the parent's type.
 *
 * @param file File that receives the new entity.
 * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue to receive the value.
 * @return Newly created IfcCostValue.
 */
IFCAPI_BINDING express::Base cost_add_cost_value(
    ifcopenshell::file* file,
    express::Base* parent);

/**
 * Create an IfcPhysicalQuantity and add it to a cost item.
 *
 * Creates a quantity of the given IFC class (e.g. "IfcQuantityLength",
 * "IfcQuantityCount") with Name "Unnamed" and appends it to the cost item's
 * CostQuantities aggregate. For IfcQuantityCount, the initial value is set
 * to the number of controlled objects.
 *
 * @param file File that receives the new entity.
 * @param cost_item IfcCostItem to receive the quantity.
 * @param ifc_class IFC quantity class name.
 * @return Newly created IfcPhysicalQuantity.
 */
IFCAPI_BINDING express::Base cost_add_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::string& ifc_class);

/**
 * Assign product quantities to a cost item.
 *
 * For each product, creates an IfcRelAssignsToControl linking the cost item
 * to the product. If prop_name is provided, matching quantities from the
 * products' IfcElementQuantity property sets are collected into the cost
 * item's CostQuantities. If prop_name is omitted or empty and the cost item has a
 * single IfcQuantityCount, its value is updated to the count of assigned
 * non-resource objects. IfcSpatialElement products are skipped.
 *
 * @param file File containing the cost item and products.
 * @param cost_item IfcCostItem to assign quantities to.
 * @param products Products whose quantities to collect.
 * @param prop_name Quantity property name to match. When omitted, no named quantity is collected.
 * @param options Ownership options for the assignment relationship.
 */
IFCAPI_BINDING void cost_assign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::vector<express::Base>& products,
    const char* prop_name,
    const CostAssignCostItemQuantityOptions& options);

/**
 * Remove product quantity assignments from a cost item.
 *
 * Removes the cost item's control relationship over the given products and
 * strips any quantities from CostQuantities that are referenced by those
 * products' IfcElementQuantity property sets. If a lone IfcQuantityCount
 * remains, its value is updated to the remaining object count or removed
 * if zero.
 *
 * @param file File containing the cost item and products.
 * @param cost_item IfcCostItem to unassign from.
 * @param products Products to unassign.
 * @param options Ownership options for updating the relationship.
 */
IFCAPI_BINDING void cost_unassign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::vector<express::Base>& products,
    const CostUnassignCostItemQuantityOptions& options);

/**
 * Remove a physical quantity from a cost item.
 *
 * If the quantity is only referenced by this cost item, it is deleted from
 * the file. Otherwise it is removed from the CostQuantities aggregate only.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem that owns the quantity.
 * @param physical_quantity IfcPhysicalQuantity to remove.
 */
IFCAPI_BINDING void cost_remove_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    express::Base* physical_quantity);

/**
 * Edit attributes of an IfcCostItem.
 *
 * Applies attribute changes from the property bag to the cost item.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void cost_edit_cost_item(
    ifcopenshell::file* file,
    express::Base* cost_item,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcCostSchedule.
 *
 * Applies attribute changes from the property bag to the cost schedule.
 *
 * @param file File containing the cost schedule.
 * @param cost_schedule IfcCostSchedule entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void cost_edit_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcPhysicalQuantity.
 *
 * Applies attribute changes from the property bag to the physical quantity.
 *
 * @param file File containing the quantity.
 * @param physical_quantity IfcPhysicalQuantity entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void cost_edit_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* physical_quantity,
    ifcopenshell_pset_props_t* attributes);

/**
 * Remove an IfcCostValue from its parent and the file.
 *
 * If the cost value is only referenced by this parent, it is deleted from
 * the file. Otherwise it is removed from the parent's CostValues, BaseCosts,
 * or Components aggregate, depending on the parent's type.
 *
 * @param file File containing the cost value.
 * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue that owns the value.
 * @param cost_value IfcCostValue to remove.
 */
IFCAPI_BINDING void cost_remove_cost_value(
    ifcopenshell::file* file,
    express::Base* parent,
    express::Base* cost_value);

/**
 * Copy the cost values from one cost item to another.
 *
 * Removes existing CostValues from the destination, then creates independent
 * copies of the source values and their component trees.
 *
 * @param file File containing both cost items.
 * @param source IfcCostItem to copy values from.
 * @param destination IfcCostItem to copy values to.
 */
IFCAPI_BINDING void cost_copy_cost_item_values(
    ifcopenshell::file* file,
    express::Base* source,
    express::Base* destination);

/**
 * Assign a cost rate's values to a cost item.
 *
 * Removes the cost item's existing CostValues, then assigns the cost rate's
 * CostValues aggregate directly (sharing, not copying).
 *
 * @param file File containing both entities.
 * @param cost_item IfcCostItem to receive the values.
 * @param cost_rate IfcCostItem whose CostValues to assign.
 */
IFCAPI_BINDING void cost_assign_cost_value(
    ifcopenshell::file* file,
    express::Base* cost_item,
    express::Base* cost_rate);

/**
 * Copy an IfcCostItem and its nested children.
 *
 * Creates independent copies of the cost item, nested child items, property
 * sets, and IfcRelDefinesByProperties relationships. The returned list contains
 * the new root item followed by its descendants.
 *
 * @param file File that receives the copied entities.
 * @param cost_item IfcCostItem to copy.
 * @return List of newly created IfcCostItem entities, with the root first.
 */
IFCAPI_BINDING std::vector<express::Base> cost_copy_cost_item(
    ifcopenshell::file* file,
    express::Base* cost_item);

/**
 * Copy an IfcCostSchedule and all its controlled cost items.
 *
 * Creates an independent schedule and independent copies of each controlled
 * IfcCostItem, then assigns the copies to the new schedule.
 *
 * @param file File that receives the copied entities.
 * @param cost_schedule IfcCostSchedule to copy.
 * @param options Ownership options for the new schedule and its items.
 * @return Newly created IfcCostSchedule.
 */
IFCAPI_BINDING express::Base cost_copy_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule,
    const CostCopyCostScheduleOptions& options);

/**
 * Remove an IfcCostItem and its nested children from the file.
 *
 * Recursively removes nested child cost items, then removes the item's
 * IfcRelNests and IfcRelAssignsToControl relationships (when the item is
 * the sole RelatedObject), and finally removes the item itself.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem to remove.
 */
IFCAPI_BINDING void cost_remove_cost_item(
    ifcopenshell::file* file,
    express::Base* cost_item);

/**
 * Remove an IfcCostSchedule and all its controlled cost items from the file.
 *
 * Removes each controlled IfcCostItem (recursively), then removes the
 * schedule's IfcRelAssignsToControl relationships and the schedule itself.
 *
 * @param file File containing the cost schedule.
 * @param cost_schedule IfcCostSchedule to remove.
 */
IFCAPI_BINDING void cost_remove_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule);

/**
 * Calculate and populate cost values from assigned resources.
 *
 * Removes existing CostValues from the cost item, then for each
 * IfcConstructionResource assigned (directly or via an IfcTask), creates
 * an IfcCostValue with the resource's base cost rate multiplied by its
 * quantity. Resources with day-based units are converted using an 8-hour
 * workday.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem to populate.
 */
IFCAPI_BINDING void cost_calculate_cost_item_resource_value(
    ifcopenshell::file* file,
    express::Base* cost_item);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COST_H */
