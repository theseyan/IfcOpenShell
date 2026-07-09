/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_MATERIAL_H
#define IFCAPI_BINDINGS_MATERIAL_H

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
 * Options for creating a material.
 */
struct MaterialAddMaterialOptions {
    std::optional<std::string> name;
    std::optional<std::string> category;
    std::optional<std::string> description;
};

/**
 * Options for creating a material set.
 */
struct MaterialAddMaterialSetOptions {
    std::optional<std::string> name;
    std::optional<std::string> set_type;
};

/**
 * Options for adding a constituent item to an IfcMaterialConstituentSet.
 */
struct MaterialAddConstituentOptions {
    express::Base material;
    std::optional<std::string> name;
};

/**
 * Options for adding a layer item to an IfcMaterialLayerSet.
 */
struct MaterialAddLayerOptions {
    express::Base material;
    std::optional<std::string> name;
};

/**
 * Options for adding a profile item to an IfcMaterialProfileSet.
 */
struct MaterialAddProfileOptions {
    std::optional<express::Base> material;
    std::optional<express::Base> profile;
    std::optional<std::string> name;
};

/**
 * Options for assigning a material, material set, or material set usage.
 */
struct MaterialAssignMaterialOptions {
    std::optional<std::string> type;
    std::optional<express::Base> material;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning material relationships.
 */
struct MaterialUnassignMaterialOptions {
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for removing a constituent or layer item.
 */
struct MaterialRemoveItemOptions {
    std::optional<bool> should_remove_material;
};

struct MaterialRemoveListItemOptions {
    std::optional<int> material_index;
};

/**
 * Options for removing a material profile item.
 */
struct MaterialRemoveProfileOptions {
    std::optional<bool> should_remove_profile_def;
    std::optional<bool> should_remove_material;
};

struct MaterialReorderSetItemOptions {
    std::optional<int> old_index;
    std::optional<int> new_index;
};

/**
 * Options for editing IfcMaterialProfileSetUsage attributes.
 */
struct MaterialEditProfileUsageOptions {
    ifcopenshell_pset_props_t* attributes = nullptr;
    std::optional<double> profile_width;
    std::optional<double> profile_height;
};

IFCAPI_BINDING express::Base material_add_constituent(
    ifcopenshell::file* file,
    express::Base* constituent_set,
    const MaterialAddConstituentOptions& options);
IFCAPI_BINDING express::Base material_add_layer(
    ifcopenshell::file* file,
    express::Base* layer_set,
    const MaterialAddLayerOptions& options);
IFCAPI_BINDING void material_add_list_item(
    ifcopenshell::file* file,
    express::Base* material_list,
    express::Base* material);
IFCAPI_BINDING express::Base material_add_material(
    ifcopenshell::file* file,
    const MaterialAddMaterialOptions& options);
IFCAPI_BINDING express::Base material_add_material_set(
    ifcopenshell::file* file,
    const MaterialAddMaterialSetOptions& options);
IFCAPI_BINDING express::Base material_add_profile(
    ifcopenshell::file* file,
    express::Base* profile_set,
    const MaterialAddProfileOptions& options);
IFCAPI_BINDING std::vector<express::Base> material_assign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    const MaterialAssignMaterialOptions& options);
IFCAPI_BINDING void material_assign_profile(
    ifcopenshell::file* file,
    express::Base* material_profile,
    express::Base* profile);
IFCAPI_BINDING void material_remove_constituent(
    ifcopenshell::file* file,
    express::Base* constituent,
    const MaterialRemoveItemOptions& options);
IFCAPI_BINDING void material_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer,
    const MaterialRemoveItemOptions& options);
IFCAPI_BINDING void material_remove_list_item(
    ifcopenshell::file* file,
    express::Base* material_list,
    const MaterialRemoveListItemOptions& options);
IFCAPI_BINDING void material_remove_material(ifcopenshell::file* file, express::Base* material);
IFCAPI_BINDING void material_remove_material_set(ifcopenshell::file* file, express::Base* material);
IFCAPI_BINDING void material_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile,
    const MaterialRemoveProfileOptions& options);
IFCAPI_BINDING void material_reorder_set_item(
    ifcopenshell::file* file,
    express::Base* material_set,
    const MaterialReorderSetItemOptions& options);
IFCAPI_BINDING void material_unassign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    const MaterialUnassignMaterialOptions& options);
IFCAPI_BINDING void material_edit_profile_usage(
    ifcopenshell::file* file,
    express::Base* usage,
    const MaterialEditProfileUsageOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_MATERIAL_H */
