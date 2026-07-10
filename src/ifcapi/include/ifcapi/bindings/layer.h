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

IFCAPI_BINDING express::Base layer_add_layer(
    ifcopenshell::file* file,
    const std::string& name);
IFCAPI_BINDING express::Base layer_add_layer_with_style(
    ifcopenshell::file* file,
    const std::string& name,
    const LayerAddLayerWithStyleOptions& options);
IFCAPI_BINDING void layer_assign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer);
IFCAPI_BINDING void layer_unassign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer);
IFCAPI_BINDING void layer_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LAYER_H */
