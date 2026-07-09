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

struct CostEditCostValueOptions {
    bool edit_unit_basis = false;
    bool clear_unit_basis = false;
    double value_component = 0.0;
    std::optional<express::Base> unit_component;
};

struct CostAddCostItemOptions {
    std::optional<express::Base> cost_schedule;
    std::optional<express::Base> cost_item;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct CostAssignCostItemQuantityOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct CostUnassignCostItemQuantityOptions {
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct CostCopyCostScheduleOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING void cost_edit_cost_value(
    ifcopenshell::file* file,
    express::Base* cost_value,
    ifcopenshell_pset_props_t* attributes,
    const CostEditCostValueOptions& options);

IFCAPI_BINDING void cost_edit_cost_value_formula(
    ifcopenshell::file* file,
    express::Base* cost_value,
    const std::string& formula);

IFCAPI_BINDING express::Base cost_add_cost_schedule(
    ifcopenshell::file* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& update_date,
    std::optional<express::Base> owner_history);

IFCAPI_BINDING express::Base cost_add_cost_item(
    ifcopenshell::file* file,
    const CostAddCostItemOptions& options);

IFCAPI_BINDING express::Base cost_add_cost_value(
    ifcopenshell::file* file,
    express::Base* parent);

IFCAPI_BINDING express::Base cost_add_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::string& ifc_class);

IFCAPI_BINDING void cost_assign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::vector<express::Base>& products,
    const char* prop_name,
    const CostAssignCostItemQuantityOptions& options);

IFCAPI_BINDING void cost_unassign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::vector<express::Base>& products,
    const CostUnassignCostItemQuantityOptions& options);

IFCAPI_BINDING void cost_remove_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    express::Base* physical_quantity);

IFCAPI_BINDING void cost_edit_cost_item(
    ifcopenshell::file* file,
    express::Base* cost_item,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void cost_edit_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void cost_edit_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* physical_quantity,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void cost_remove_cost_value(
    ifcopenshell::file* file,
    express::Base* parent,
    express::Base* cost_value);

IFCAPI_BINDING void cost_copy_cost_item_values(
    ifcopenshell::file* file,
    express::Base* source,
    express::Base* destination);

IFCAPI_BINDING void cost_assign_cost_value(
    ifcopenshell::file* file,
    express::Base* cost_item,
    express::Base* cost_rate);

IFCAPI_BINDING IFCAPI_OWNED std::vector<express::Base> cost_copy_cost_item(
    ifcopenshell::file* file,
    express::Base* cost_item);

IFCAPI_BINDING express::Base cost_copy_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule,
    const CostCopyCostScheduleOptions& options);

IFCAPI_BINDING void cost_remove_cost_item(
    ifcopenshell::file* file,
    express::Base* cost_item);

IFCAPI_BINDING void cost_remove_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule);

IFCAPI_BINDING void cost_calculate_cost_item_resource_value(
    ifcopenshell::file* file,
    express::Base* cost_item);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COST_H */
