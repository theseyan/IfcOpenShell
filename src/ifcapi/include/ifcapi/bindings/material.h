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
    /// Optional material name. Defaults to "Unnamed" if omitted.
    std::optional<std::string> name;
    /// Optional category (e.g. "Concrete", "Steel").
    std::optional<std::string> category;
    /// Optional description.
    std::optional<std::string> description;
};

/**
 * Options for creating a material set.
 */
struct MaterialAddMaterialSetOptions {
    /// Optional name for the set. Defaults to "Unnamed" if omitted.
    std::optional<std::string> name;
    /// IFC class of the set to create. Defaults to "IfcMaterialConstituentSet".
    /// Valid values: "IfcMaterialConstituentSet", "IfcMaterialLayerSet",
    /// "IfcMaterialProfileSet", "IfcMaterialList".
    std::optional<std::string> set_type;
};

/**
 * Options for adding a constituent item to an IfcMaterialConstituentSet.
 */
struct MaterialAddConstituentOptions {
    /// The IfcMaterial for this constituent.
    express::Base material;
    /// Optional name for the IfcMaterialConstituent.
    std::optional<std::string> name;
};

/**
 * Options for adding a layer item to an IfcMaterialLayerSet.
 */
struct MaterialAddLayerOptions {
    /// The IfcMaterial for this layer.
    express::Base material;
    /// Optional name for the IfcMaterialLayer (ignored in IFC2X3).
    std::optional<std::string> name;
};

/**
 * Options for adding a profile item to an IfcMaterialProfileSet.
 */
struct MaterialAddProfileOptions {
    /// Optional IfcMaterial for this profile. Omitted if not set.
    std::optional<express::Base> material;
    /// Optional IfcProfileDef for this profile. Omitted if not set.
    std::optional<express::Base> profile;
    /// Optional name for the IfcMaterialProfile.
    std::optional<std::string> name;
};

/**
 * Options for assigning a material, material set, or material set usage.
 */
struct MaterialAssignMaterialOptions {
    /// Material type to assign. Defaults to "IfcMaterial". Valid values:
    /// "IfcMaterial", "IfcMaterialConstituentSet", "IfcMaterialLayerSet",
    /// "IfcMaterialProfileSet", "IfcMaterialLayerSetUsage",
    /// "IfcMaterialProfileSetUsage", "IfcMaterialList".
    std::optional<std::string> type;
    /// Optional existing material or material set to use. For usage types,
    /// must be the corresponding set type (e.g. IfcMaterialLayerSet for
    /// IfcMaterialLayerSetUsage). If omitted, a new set is created.
    std::optional<express::Base> material;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning material relationships.
 */
struct MaterialUnassignMaterialOptions {
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Options for removing a constituent or layer item.
 */
struct MaterialRemoveItemOptions {
    /// If true, the associated IfcMaterial is also deleted (deep removal). Defaults to false.
    std::optional<bool> should_remove_material;
};

/**
 * Options for removing an item from an IfcMaterialList.
 */
struct MaterialRemoveListItemOptions {
    /// Zero-based index of the item to remove. Defaults to 0.
    std::optional<int> material_index;
};

/**
 * Options for removing a material profile item.
 */
struct MaterialRemoveProfileOptions {
    /// If true, the associated IfcProfileDef is also deleted. Defaults to false.
    std::optional<bool> should_remove_profile_def;
    /// If true, the associated IfcMaterial is also deleted. Defaults to false.
    std::optional<bool> should_remove_material;
};

/**
 * Options for reordering items within a material set.
 */
struct MaterialReorderSetItemOptions {
    /// Zero-based index of the item to move. Defaults to 0.
    std::optional<int> old_index;
    /// Zero-based index of the destination position. Defaults to 0.
    std::optional<int> new_index;
};

/**
 * Options for editing IfcMaterialProfileSetUsage attributes.
 */
struct MaterialEditProfileUsageOptions {
    /// Attribute key-value pairs to apply to the usage entity.
    ifcopenshell_pset_props_t* attributes = nullptr;
    /// Profile width in model units, used for cardinal point placement.
    /// Required along with profile_height when changing CardinalPoint.
    std::optional<double> profile_width;
    /// Profile height in model units, used for cardinal point placement.
    /// Required along with profile_width when changing CardinalPoint.
    std::optional<double> profile_height;
};

/**
 * Add an IfcMaterialConstituent to an IfcMaterialConstituentSet.
 */
IFCAPI_BINDING express::Base material_add_constituent(
    ifcopenshell::file* file,
    express::Base* constituent_set,
    const MaterialAddConstituentOptions& options);

/**
 * Add an IfcMaterialLayer to an IfcMaterialLayerSet.
 *
 * The layer thickness is initialized to 0.1 metres, converted to the file's
 * project units (e.g. 100 for a millimetre project).
 */
IFCAPI_BINDING express::Base material_add_layer(
    ifcopenshell::file* file,
    express::Base* layer_set,
    const MaterialAddLayerOptions& options);

/**
 * Append an IfcMaterial to an IfcMaterialList.
 */
IFCAPI_BINDING void material_add_list_item(
    ifcopenshell::file* file,
    express::Base* material_list,
    express::Base* material);

/**
 * Create a new IfcMaterial.
 */
IFCAPI_BINDING express::Base material_add_material(
    ifcopenshell::file* file,
    const MaterialAddMaterialOptions& options);

/**
 * Create a new material set of the requested type.
 *
 * Defaults to IfcMaterialConstituentSet if set_type is omitted.
 */
IFCAPI_BINDING express::Base material_add_material_set(
    ifcopenshell::file* file,
    const MaterialAddMaterialSetOptions& options);

/**
 * Add an IfcMaterialProfile to an IfcMaterialProfileSet.
 */
IFCAPI_BINDING express::Base material_add_profile(
    ifcopenshell::file* file,
    express::Base* profile_set,
    const MaterialAddProfileOptions& options);

/**
 * Assign a material to products via IfcRelAssociatesMaterial.
 *
 * Any existing material assignment on the products is removed first.
 * For "IfcMaterialLayerSetUsage" and "IfcMaterialProfileSetUsage", the
 * products are grouped by material set and direction; each group gets its
 * own usage entity and relationship. Returns the created relationship(s).
 */
IFCAPI_BINDING std::vector<express::Base> material_assign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    const MaterialAssignMaterialOptions& options);

/**
 * Replace the profile on an IfcMaterialProfile and update representations.
 *
 * Sets the Profile attribute on the material profile. Updates the
 * SweptArea of any IfcSweptAreaSolid in representations of elements
 * using the parent IfcMaterialProfileSetUsage. The old profile is
 * deleted if it has no remaining references.
 */
IFCAPI_BINDING void material_assign_profile(
    ifcopenshell::file* file,
    express::Base* material_profile,
    express::Base* profile);

/**
 * Remove an IfcMaterialConstituent from its set.
 *
 * If should_remove_material is true, the associated IfcMaterial is also
 * deleted.
 */
IFCAPI_BINDING void material_remove_constituent(
    ifcopenshell::file* file,
    express::Base* constituent,
    const MaterialRemoveItemOptions& options);

/**
 * Remove an IfcMaterialLayer from its set.
 *
 * If should_remove_material is true, the associated IfcMaterial is also
 * deleted.
 */
IFCAPI_BINDING void material_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer,
    const MaterialRemoveItemOptions& options);

/**
 * Remove an item from an IfcMaterialList by index.
 */
IFCAPI_BINDING void material_remove_list_item(
    ifcopenshell::file* file,
    express::Base* material_list,
    const MaterialRemoveListItemOptions& options);

/**
 * Remove an IfcMaterial and its container constituents/layers/profiles.
 *
 * Deletes the material entity. Constituent, layer, or profile entities
 * that reference it are also removed. Associated IfcRelAssociatesMaterial,
 * IfcMaterialProperties, and IfcMaterialDefinitionRepresentation entities
 * are cleaned up.
 */
IFCAPI_BINDING void material_remove_material(ifcopenshell::file* file, express::Base* material);

/**
 * Remove a material set and its items.
 *
 * Deletes all items (layers, profiles, constituents) in the set, then
 * removes the set entity. For IfcMaterialLayerSet and IfcMaterialProfileSet,
 * associated usage entities and their relationships are also deleted.
 * Associated IfcRelAssociatesMaterial and IfcMaterialProperties are
 * cleaned up.
 */
IFCAPI_BINDING void material_remove_material_set(ifcopenshell::file* file, express::Base* material);

/**
 * Remove an IfcMaterialProfile from its set.
 *
 * Optionally deletes the associated IfcMaterial and/or IfcProfileDef
 * if the corresponding flags are set.
 */
IFCAPI_BINDING void material_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile,
    const MaterialRemoveProfileOptions& options);

/**
 * Reorder an item within a material set.
 *
 * Moves the item from old_index to new_index within the set's aggregate
 * (MaterialConstituents, MaterialLayers, MaterialProfiles, or Materials).
 * Supports IfcMaterialConstituentSet, IfcMaterialLayerSet,
 * IfcMaterialProfileSet, and IfcMaterialList.
 */
IFCAPI_BINDING void material_reorder_set_item(
    ifcopenshell::file* file,
    express::Base* material_set,
    const MaterialReorderSetItemOptions& options);

/**
 * Remove material assignments from products.
 *
 * Deletes IfcRelAssociatesMaterial relationships for the products. If the
 * material is an IfcMaterialLayerSetUsage or IfcMaterialProfileSetUsage
 * with no remaining products, the usage entity is also deleted.
 */
IFCAPI_BINDING void material_unassign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    const MaterialUnassignMaterialOptions& options);

/**
 * Edit attributes of an IfcMaterialProfileSetUsage.
 *
 * Applies attribute key-value pairs from the props builder. If CardinalPoint
 * changes and both profile_width and profile_height are provided, the
 * ObjectPlacement of associated elements is updated to reflect the new
 * cardinal point position.
 */
IFCAPI_BINDING void material_edit_profile_usage(
    ifcopenshell::file* file,
    express::Base* usage,
    const MaterialEditProfileUsageOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_MATERIAL_H */
