// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_STRUCTURAL_H
#define IFCAPI_BINDINGS_STRUCTURAL_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_analysis_model(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_assign_structural_analysis_model(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void structural_unassign_structural_analysis_model(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_assign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_activity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* applied_load,
    IfcUtil::IfcBaseClass* structural_member,
    const std::string& ifc_class,
    const std::string& predefined_type,
    const std::string& global_or_local,
    IfcUtil::IfcBaseClass* activity_owner_history,
    IfcUtil::IfcBaseClass* relationship_owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_load(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const char* name,
    bool has_name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_load_case(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_load_group(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& action_type,
    const std::string& action_source,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_member_connection(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_structural_member,
    IfcUtil::IfcBaseClass* related_structural_connection,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_boundary_condition(
    IfcParse::IfcFile* file,
    const char* name,
    bool has_name,
    IfcUtil::IfcBaseClass* connection,
    const std::string& ifc_class);
IFCAPI_BINDING void structural_edit_structural_connection_cs(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_item,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction);
IFCAPI_BINDING void structural_edit_structural_item_axis(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_item,
    const std::vector<double>& axis);
IFCAPI_BINDING void structural_edit_structural_boundary_condition(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* condition,
    ifcopenshell_pset_props_t* attributes);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_assign_to_building(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* building,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING void structural_remove_structural_analysis_model(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_analysis_model);
IFCAPI_BINDING void structural_remove_structural_load(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_load);
IFCAPI_BINDING void structural_remove_structural_load_case(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_load_case);
IFCAPI_BINDING void structural_remove_structural_load_group(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* structural_load_group);
IFCAPI_BINDING void structural_remove_structural_boundary_condition(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* connection,
    IfcUtil::IfcBaseClass* boundary_condition);
IFCAPI_BINDING void structural_remove_structural_connection_condition(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relation);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STRUCTURAL_H */
