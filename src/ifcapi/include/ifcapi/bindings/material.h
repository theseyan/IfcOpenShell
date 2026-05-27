/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_MATERIAL_H
#define IFCAPI_BINDINGS_MATERIAL_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base material_add_constituent(
    ifcopenshell::file* file,
    express::Base* constituent_set,
    express::Base* material,
    const char* name);
IFCAPI_BINDING express::Base material_add_layer(
    ifcopenshell::file* file,
    express::Base* layer_set,
    express::Base* material,
    const char* name);
IFCAPI_BINDING void material_add_list_item(
    ifcopenshell::file* file,
    express::Base* material_list,
    express::Base* material);
IFCAPI_BINDING express::Base material_add_material(
    ifcopenshell::file* file,
    const char* name,
    const char* category,
    const char* description);
IFCAPI_BINDING express::Base material_add_material_set(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& set_type);
IFCAPI_BINDING express::Base material_add_profile(
    ifcopenshell::file* file,
    express::Base* profile_set,
    IFCAPI_NULLABLE express::Base* material,
    IFCAPI_NULLABLE express::Base* profile,
    const char* name);
IFCAPI_BINDING std::vector<express::Base> material_assign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    const std::string& type,
    IFCAPI_NULLABLE express::Base* material,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void material_assign_profile(
    ifcopenshell::file* file,
    express::Base* material_profile,
    express::Base* profile);
IFCAPI_BINDING void material_remove_constituent(
    ifcopenshell::file* file,
    express::Base* constituent,
    bool should_remove_material);
IFCAPI_BINDING void material_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer,
    bool should_remove_material);
IFCAPI_BINDING void material_remove_list_item(
    ifcopenshell::file* file,
    express::Base* material_list,
    int material_index);
IFCAPI_BINDING void material_remove_material(ifcopenshell::file* file, express::Base* material);
IFCAPI_BINDING void material_remove_material_set(ifcopenshell::file* file, express::Base* material);
IFCAPI_BINDING void material_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile,
    bool should_remove_profile_def,
    bool should_remove_material);
IFCAPI_BINDING void material_reorder_set_item(
    ifcopenshell::file* file,
    express::Base* material_set,
    int old_index,
    int new_index);
IFCAPI_BINDING void material_unassign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void material_edit_profile_usage(
    ifcopenshell::file* file,
    express::Base* usage,
    ifcopenshell_pset_props_t* attributes,
    bool has_profile_dimensions,
    double profile_width,
    double profile_height);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_MATERIAL_H */
