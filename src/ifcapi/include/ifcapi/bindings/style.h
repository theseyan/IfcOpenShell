/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_STYLE_H
#define IFCAPI_BINDINGS_STYLE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base style_add_style(
    ifcopenshell::file* file,
    const char* name,
    const std::string& ifc_class);
IFCAPI_BINDING express::Base style_assign_item_style(
    ifcopenshell::file* file,
    express::Base* item,
    IFCAPI_NULLABLE express::Base* style,
    bool should_use_presentation_style_assignment);
IFCAPI_BINDING std::vector<express::Base> style_assign_representation_styles(
    ifcopenshell::file* file,
    express::Base* shape_representation,
    const std::vector<express::Base>& styles,
    bool should_use_presentation_style_assignment,
    bool replace_previous_same_type_style);
IFCAPI_BINDING void style_assign_material_style(
    ifcopenshell::file* file,
    express::Base* material,
    express::Base* style,
    express::Base* context,
    bool should_use_presentation_style_assignment);
IFCAPI_BINDING void style_edit_surface_style(
    ifcopenshell::file* file,
    express::Base* style,
    ifcopenshell_pset_props_t* attributes);
IFCAPI_BINDING void style_remove_style(ifcopenshell::file* file, express::Base* style);
IFCAPI_BINDING void style_remove_styled_representation(
    ifcopenshell::file* file,
    express::Base* representation);
IFCAPI_BINDING void style_remove_surface_style(ifcopenshell::file* file, express::Base* style);
IFCAPI_BINDING void style_unassign_representation_styles(
    ifcopenshell::file* file,
    express::Base* shape_representation,
    const std::vector<express::Base>& styles,
    bool should_use_presentation_style_assignment);
IFCAPI_BINDING void style_unassign_material_style(
    ifcopenshell::file* file,
    express::Base* material,
    express::Base* style,
    express::Base* context);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STYLE_H */
