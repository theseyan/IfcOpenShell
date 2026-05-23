/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_STYLE_H
#define IFCAPI_BINDINGS_STYLE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* style_add_style(
    IfcParse::IfcFile* file,
    const char* name,
    const std::string& ifc_class);
IFCAPI_BINDING IfcUtil::IfcBaseClass* style_assign_item_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* style,
    bool should_use_presentation_style_assignment);
IFCAPI_BINDING std::vector<IfcUtil::IfcBaseClass*> style_assign_representation_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* shape_representation,
    const std::vector<const IfcUtil::IfcBaseClass*>& styles,
    bool should_use_presentation_style_assignment,
    bool replace_previous_same_type_style);
IFCAPI_BINDING void style_assign_material_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* style,
    IfcUtil::IfcBaseClass* context,
    bool should_use_presentation_style_assignment);
IFCAPI_BINDING void style_edit_surface_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* style,
    ifcopenshell_pset_props_t* attributes);
IFCAPI_BINDING void style_remove_style(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style);
IFCAPI_BINDING void style_remove_styled_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING void style_remove_surface_style(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style);
IFCAPI_BINDING void style_unassign_representation_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* shape_representation,
    const std::vector<const IfcUtil::IfcBaseClass*>& styles,
    bool should_use_presentation_style_assignment);
IFCAPI_BINDING void style_unassign_material_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* style,
    IfcUtil::IfcBaseClass* context);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STYLE_H */
