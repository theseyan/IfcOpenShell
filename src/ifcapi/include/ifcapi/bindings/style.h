/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_STYLE_H
#define IFCAPI_BINDINGS_STYLE_H

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
 * Create a new presentation style entity.
 *
 * For IfcSurfaceStyle, the Side attribute defaults to "BOTH".
 *
 * @param file IFC file that receives the style.
 * @param name Style name (may be null for unnamed styles).
 * @param ifc_class IFC entity class (e.g. "IfcSurfaceStyle", "IfcFillAreaStyle").
 * @return Newly created style entity.
 */
IFCAPI_BINDING express::Base style_add_style(
    ifcopenshell::file* file,
    const char* name,
    const std::string& ifc_class);

/**
 * Options for assigning a style to a representation item.
 */
struct StyleAssignItemStyleOptions {
    /// The representation item to assign the style to.
    express::Base item;
    /// Optional style to assign. Empty to remove/unassign.
    std::optional<express::Base> style;
    /// Whether to use IfcPresentationStyleAssignment (for IFC2X3 compat).
    bool should_use_presentation_style_assignment;
};

/**
 * Assign or replace a style on a single representation item.
 *
 * Creates an IfcStyledItem (and optionally an IfcPresentationStyleAssignment
 * for IFC2X3) linking the item to the given style. If the item already has a
 * styled item, the existing style is replaced. Passing an empty style removes
 * the styled item from the representation item.
 *
 * @param file IFC file to modify.
 * @param options Item, style, and IFC2X3 compat flag.
 * @return The IfcStyledItem, or null handle if style was removed.
 */
IFCAPI_BINDING express::Base style_assign_item_style(
    ifcopenshell::file* file,
    const StyleAssignItemStyleOptions& options);

/**
 * Assign styles to the geometric items within a shape representation.
 *
 * Traverses the representation and assigns each style to sequential
 * representation items. When replace_previous_same_type_style is true, styles
 * of the same IFC class are replaced rather than appended.
 *
 * @param file IFC file to modify.
 * @param shape_representation IfcShapeRepresentation to assign styles to.
 * @param styles Presentation style entities to assign.
 * @param should_use_presentation_style_assignment Wrap styles in IfcPresentationStyleAssignment.
 * @param replace_previous_same_type_style Replace existing styles of the same type.
 * @return Vector of newly created IfcStyledItem entities.
 */
IFCAPI_BINDING std::vector<express::Base> style_assign_representation_styles(
    ifcopenshell::file* file,
    express::Base* shape_representation,
    const std::vector<express::Base>& styles,
    bool should_use_presentation_style_assignment,
    bool replace_previous_same_type_style);

/**
 * Assign a style to a material via IfcMaterialDefinitionRepresentation.
 *
 * Creates or extends a styled representation under the material's definition.
 * Also propagates the style to shape aspects whose names match material
 * constituent names.
 *
 * @param file IFC file to modify.
 * @param material IfcMaterial entity.
 * @param style Presentation style entity to assign.
 * @param context IfcGeometricRepresentationContext for the styled representation.
 * @param should_use_presentation_style_assignment Wrap style in IfcPresentationStyleAssignment.
 */
IFCAPI_BINDING void style_assign_material_style(
    ifcopenshell::file* file,
    express::Base* material,
    express::Base* style,
    express::Base* context,
    bool should_use_presentation_style_assignment);

/**
 * Edit attributes of an IfcSurfaceStyle or its nested surface styles.
 *
 * Handles colour attributes (IfcColourRgb), specular highlights, and
 * colour-or-factor properties specially; other attributes are set directly.
 *
 * @param file IFC file containing the style.
 * @param style IfcSurfaceStyle entity to modify.
 * @param attributes Property container with attribute name-value pairs.
 */
IFCAPI_BINDING void style_edit_surface_style(
    ifcopenshell::file* file,
    express::Base* style,
    ifcopenshell_pset_props_t* attributes);

/**
 * Remove a presentation style and clean up all references to it.
 *
 * Removes inverse IfcStyledItem references, nested surface styles (for
 * IfcSurfaceStyle), fill area style hatching, and orphaned
 * IfcPresentationStyleAssignment entities.
 *
 * @param file IFC file to modify.
 * @param style Presentation style entity to remove.
 */
IFCAPI_BINDING void style_remove_style(ifcopenshell::file* file, express::Base* style);

/**
 * Remove an IfcStyledRepresentation and its parent IfcMaterialDefinitionRepresentation.
 *
 * @param file IFC file to modify.
 * @param representation IfcStyledRepresentation entity to remove.
 */
IFCAPI_BINDING void style_remove_styled_representation(
    ifcopenshell::file* file,
    express::Base* representation);

/**
 * Remove an IfcSurfaceStyleWithTextures or IfcSurfaceStyleRendering and its nested entities.
 *
 * Deep-removes texture coordinates, textures, and colour entities owned by the
 * surface style.
 *
 * @param file IFC file to modify.
 * @param style Surface style sub-entity to remove.
 */
IFCAPI_BINDING void style_remove_surface_style(ifcopenshell::file* file, express::Base* style);

/**
 * Remove styles from the geometric items within a shape representation.
 *
 * Traverses the representation and removes matching styles from IfcStyledItem
 * and IfcPresentationStyleAssignment entities.
 *
 * @param file IFC file to modify.
 * @param shape_representation IfcShapeRepresentation to unassign styles from.
 * @param styles Presentation style entities to remove.
 * @param should_use_presentation_style_assignment Also search inside IfcPresentationStyleAssignment.
 */
IFCAPI_BINDING void style_unassign_representation_styles(
    ifcopenshell::file* file,
    express::Base* shape_representation,
    const std::vector<express::Base>& styles,
    bool should_use_presentation_style_assignment);

/**
 * Remove a style from a material's styled representation.
 *
 * Cleans up empty IfcStyledItem, IfcStyledRepresentation, and
 * IfcMaterialDefinitionRepresentation entities. Also propagates removal to
 * matching shape aspects.
 *
 * @param file IFC file to modify.
 * @param material IfcMaterial entity.
 * @param style Presentation style entity to remove.
 * @param context IfcGeometricRepresentationContext identifying the styled representation.
 */
IFCAPI_BINDING void style_unassign_material_style(
    ifcopenshell::file* file,
    express::Base* material,
    express::Base* style,
    express::Base* context);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STYLE_H */
