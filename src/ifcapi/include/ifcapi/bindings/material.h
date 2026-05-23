/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_MATERIAL_H
#define IFCAPI_BINDINGS_MATERIAL_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* material_add_constituent(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* constituent_set,
    IfcUtil::IfcBaseClass* material,
    const char* name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* material_add_layer(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* layer_set,
    IfcUtil::IfcBaseClass* material,
    const char* name);
IFCAPI_BINDING void material_add_list_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material_list,
    IfcUtil::IfcBaseClass* material);
IFCAPI_BINDING IfcUtil::IfcBaseClass* material_add_material(
    IfcParse::IfcFile* file,
    const char* name,
    const char* category,
    const char* description);
IFCAPI_BINDING IfcUtil::IfcBaseClass* material_add_material_set(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& set_type);
IFCAPI_BINDING IfcUtil::IfcBaseClass* material_add_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile_set,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* material,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* profile,
    const char* name);
IFCAPI_BINDING std::vector<IfcUtil::IfcBaseClass*> material_assign_material(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    const std::string& type,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* material,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void material_assign_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material_profile,
    IfcUtil::IfcBaseClass* profile);
IFCAPI_BINDING void material_remove_constituent(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* constituent,
    bool should_remove_material);
IFCAPI_BINDING void material_remove_layer(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* layer,
    bool should_remove_material);
IFCAPI_BINDING void material_remove_list_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material_list,
    int material_index);
IFCAPI_BINDING void material_remove_material(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* material);
IFCAPI_BINDING void material_remove_material_set(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* material);
IFCAPI_BINDING void material_remove_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile,
    bool should_remove_profile_def,
    bool should_remove_material);
IFCAPI_BINDING void material_reorder_set_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material_set,
    int old_index,
    int new_index);
IFCAPI_BINDING void material_unassign_material(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void material_edit_profile_usage(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* usage,
    ifcopenshell_pset_props_t* attributes,
    bool has_profile_dimensions,
    double profile_width,
    double profile_height);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_MATERIAL_H */
