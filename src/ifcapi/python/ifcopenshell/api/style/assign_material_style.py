# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.style
import ifcopenshell.util.element


def assign_material_style(file, material=None, style=None, context=None,
                          should_use_presentation_style_assignment=False):
    wrapped_style = style
    if file.schema == "IFC2X3" or should_use_presentation_style_assignment:
        wrapped_style = file.createIfcPresentationStyleAssignment([style])

    if material.HasRepresentation:
        _modify_existing_definition_representation(
            file, material, style, wrapped_style, context,
            should_use_presentation_style_assignment)
    else:
        _create_new_definition_representation(file, material, style, wrapped_style, context)

    # Handle material constituents and shape aspects.
    constituent_names = []
    for inverse in file.get_inverse(material):
        if inverse.is_a("IfcMaterialConstituent") and inverse.Name:
            constituent_names.append(inverse.Name)
    if not constituent_names:
        return

    elements = _get_elements_by_material(file, material)
    shape_aspects = []
    for element in elements:
        shape_aspects += _get_shape_aspects(file, element)

    for shape_aspect in shape_aspects:
        if shape_aspect.Name not in constituent_names:
            continue
        for rep in shape_aspect.ShapeRepresentations:
            ifcopenshell.api.style.assign_representation_styles(
                file, shape_representation=rep, styles=[wrapped_style])


def _modify_existing_definition_representation(file, material, style, wrapped_style,
                                                context, use_psa):
    definition_representation = material.HasRepresentation[0]
    representation = _get_styled_representation(definition_representation, context)
    if representation:
        items = list(representation.Items)
        new_items = []
        same_style_items = []
        for item in items:
            if not item.is_a("IfcStyledItem"):
                continue
            if _has_proposed_style(file, item, style):
                return
            if _has_same_style_type(item, style):
                same_style_items.append(item)
            else:
                new_items.append(item)
        item_to_reuse = same_style_items.pop(0) if same_style_items else None
        new_items.append(_create_styled_item(file, style, wrapped_style, item_to_reuse))
        representation.Items = new_items
        for item in same_style_items:
            if file.get_total_inverses(item) == 0:
                file.remove(item)
    else:
        representations = list(definition_representation.Representations)
        representations.append(_create_styled_representation(file, style, wrapped_style, context))
        definition_representation.Representations = representations


def _has_proposed_style(file, styled_item, style):
    styles = styled_item.Styles
    if style in styles:
        return True
    if file.schema != "IFC4X3":
        for s in styles:
            if s.is_a("IfcPresentationStyleAssignment"):
                if style in s.Styles:
                    return True
    return False


def _has_same_style_type(styled_item, style):
    style_class = style.is_a()
    for s in styled_item.Styles:
        s_class = s.is_a()
        if s_class == style_class:
            return True
        elif s_class == "IfcPresentationStyleAssignment":
            for ss in s.Styles:
                if ss.is_a() == style_class:
                    return True
    return False


def _create_new_definition_representation(file, material, style, wrapped_style, context):
    representation = _create_styled_representation(file, style, wrapped_style, context)
    file.create_entity(
        "IfcMaterialDefinitionRepresentation",
        Representations=[representation],
        RepresentedMaterial=material)


def _get_styled_representation(definition_representation, context):
    for r in definition_representation.Representations:
        if r.is_a("IfcStyledRepresentation") and r.ContextOfItems == context:
            return r
    return None


def _create_styled_representation(file, style, wrapped_style, context):
    return file.create_entity(
        "IfcStyledRepresentation",
        ContextOfItems=context,
        RepresentationIdentifier=context.ContextIdentifier,
        Items=[_create_styled_item(file, style, wrapped_style)])


def _create_styled_item(file, style, wrapped_style, reuse_item=None):
    if reuse_item is None:
        return file.create_entity("IfcStyledItem", Styles=[wrapped_style], Name=style.Name)

    if (reuse_item.is_a("IfcPresentationStyleAssignment")
            and wrapped_style.is_a("IfcPresentationStyleAssignment")):
        file.remove(wrapped_style)
        wrapped_style = reuse_item

    reuse_item.Styles = (style,)
    reuse_item.Name = style.Name
    return reuse_item


# -- Inline helpers for util functions not present in this package -----------

def _get_elements_by_material(ifc_file, material):
    """Simplified get_elements_by_material."""
    results = set()
    for inverse in ifc_file.get_inverse(material):
        if inverse.is_a("IfcRelAssociatesMaterial"):
            results.update(inverse.RelatedObjects or [])
        elif inverse.is_a("IfcMaterialLayer"):
            for material_set in inverse.ToMaterialLayerSet:
                results.update(_get_elements_by_material(ifc_file, material_set))
        elif inverse.is_a("IfcMaterialProfile"):
            for material_set in inverse.ToMaterialProfileSet:
                results.update(_get_elements_by_material(ifc_file, material_set))
        elif inverse.is_a("IfcMaterialConstituent"):
            for material_set in inverse.ToMaterialConstituentSet:
                results.update(_get_elements_by_material(ifc_file, material_set))
        elif inverse.is_a("IfcMaterialLayerSetUsage"):
            results.update(_get_elements_by_material(ifc_file, inverse))
        elif inverse.is_a("IfcMaterialProfileSetUsage"):
            results.update(_get_elements_by_material(ifc_file, inverse))
        elif inverse.is_a("IfcMaterialList"):
            results.update(_get_elements_by_material(ifc_file, inverse))
    return results


def _get_shape_aspects(ifc_file, element):
    """Simplified get_shape_aspects for an IfcProduct."""
    representation = getattr(element, "Representation", None)
    if representation is None:
        return []
    shape_aspects = list(representation.HasShapeAspects or [])
    element_type = ifcopenshell.util.element.get_type(element)
    if element_type:
        for rm in getattr(element_type, "RepresentationMaps", ()) or ():
            shape_aspects += list(rm.HasShapeAspects or [])
    return shape_aspects
