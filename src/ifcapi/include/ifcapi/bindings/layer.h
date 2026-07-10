// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_LAYER_H
#define IFCAPI_BINDINGS_LAYER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a presentation layer with style.
 */
struct LayerAddLayerWithStyleOptions {
    /// Whether the layer is visible. nullopt maps to IFC UNKNOWN.
    std::optional<bool> on;
    /// Whether the layer is frozen. nullopt maps to IFC UNKNOWN.
    std::optional<bool> frozen;
    /// Whether the layer is blocked. nullopt maps to IFC UNKNOWN.
    std::optional<bool> blocked;
    /// Styles to assign to the layer.
    std::vector<express::Base> styles;
};

/**
 * Create an IfcPresentationLayerAssignment with the given name.
 *
 * @param file IFC file that receives the layer.
 * @param name Layer name.
 * @return Newly created IfcPresentationLayerAssignment.
 */
IFCAPI_BINDING express::Base layer_add_layer(
    ifcopenshell::file* file,
    const std::string& name);

/**
 * Create an IfcPresentationLayerWithStyle with visibility flags and styles.
 *
 * @param file IFC file that receives the layer.
 * @param name Layer name.
 * @param options Visibility flags and styles.
 * @return Newly created IfcPresentationLayerWithStyle.
 */
IFCAPI_BINDING express::Base layer_add_layer_with_style(
    ifcopenshell::file* file,
    const std::string& name,
    const LayerAddLayerWithStyleOptions& options);

/**
 * Assign representation items to a presentation layer.
 *
 * Items already present in the layer's AssignedItems are not duplicated.
 *
 * @param file IFC file (unused, reserved for consistency).
 * @param items Representation items to assign.
 * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
 */
IFCAPI_BINDING void layer_assign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer);

/**
 * Remove representation items from a presentation layer.
 *
 * If all assigned items are removed, the layer entity itself is deleted from
 * the file. Does nothing silently when any specified item is not currently
 * assigned to the layer.
 *
 * @param file IFC file to modify.
 * @param items Representation items to unassign.
 * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
 */
IFCAPI_BINDING void layer_unassign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer);

/**
 * Remove a presentation layer entity from the file.
 *
 * @param file IFC file to modify.
 * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle to remove.
 */
IFCAPI_BINDING void layer_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LAYER_H */
