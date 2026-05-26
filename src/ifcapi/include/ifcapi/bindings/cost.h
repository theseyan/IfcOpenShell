// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COST_H
#define IFCAPI_BINDINGS_COST_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void cost_edit_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_value,
    ifcopenshell_pset_props_t* attributes,
    bool has_unit_basis,
    bool unit_basis_is_null,
    double value_component,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* unit_component);

IFCAPI_BINDING void cost_edit_cost_value_formula(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_value,
    const std::string& formula);

IFCAPI_BINDING IfcUtil::IfcBaseClass* cost_add_cost_schedule(
    IfcParse::IfcFile* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& update_date,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history);

IFCAPI_BINDING IfcUtil::IfcBaseClass* cost_add_cost_item(
    IfcParse::IfcFile* file,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* cost_schedule,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* cost_item,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* cost_add_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent);

IFCAPI_BINDING IfcUtil::IfcBaseClass* cost_add_cost_item_quantity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item,
    const std::string& ifc_class);

IFCAPI_BINDING void cost_assign_cost_item_quantity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    const char* prop_name,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING void cost_unassign_cost_item_quantity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING void cost_remove_cost_item_quantity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item,
    IfcUtil::IfcBaseClass* physical_quantity);

IFCAPI_BINDING void cost_edit_cost_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void cost_edit_cost_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_schedule,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void cost_edit_cost_item_quantity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* physical_quantity,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void cost_remove_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent,
    IfcUtil::IfcBaseClass* cost_value);

IFCAPI_BINDING void cost_copy_cost_item_values(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* source,
    IfcUtil::IfcBaseClass* destination);

IFCAPI_BINDING void cost_assign_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item,
    IfcUtil::IfcBaseClass* cost_rate);

IFCAPI_BINDING IFCAPI_OWNED std::vector<IfcUtil::IfcBaseClass*> cost_copy_cost_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item);

IFCAPI_BINDING IfcUtil::IfcBaseClass* cost_copy_cost_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_schedule,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING void cost_remove_cost_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item);

IFCAPI_BINDING void cost_remove_cost_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_schedule);

IFCAPI_BINDING void cost_calculate_cost_item_resource_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_item);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COST_H */
