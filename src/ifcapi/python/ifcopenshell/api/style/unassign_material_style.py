# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.style
import ifcopenshell.util.element


def unassign_material_style(file, material=None, style=None, context=None):
    for definition in material.HasRepresentation:
        for representation in definition.Representations:
            if not representation.is_a("IfcStyledRepresentation"):
                continue
            if representation.ContextOfItems != context:
                continue
            for item in representation.Items:
                if not item.is_a("IfcStyledItem"):
                    continue
                styles = []
                for s in item.Styles:
                    if s == style:
                        continue
                    if s.is_a("IfcPresentationStyleAssignment"):
                        if s.Styles == (style,):
                            continue
                    styles.append(s)
                if not styles:
                    file.remove(item)
                elif len(styles) != len(item.Styles):
                    item.Styles = styles
            if not representation.Items:
                file.remove(representation)
        if not definition.Representations:
            file.remove(definition)

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
            ifcopenshell.api.style.unassign_representation_styles(
                file, shape_representation=rep, styles=[style])


def _get_elements_by_material(ifc_file, material):
    results = set()
    for inverse in ifc_file.get_inverse(material):
        if inverse.is_a("IfcRelAssociatesMaterial"):
            results.update(inverse.RelatedObjects or [])
        elif inverse.is_a("IfcMaterialLayer"):
            for ms in inverse.ToMaterialLayerSet:
                results.update(_get_elements_by_material(ifc_file, ms))
        elif inverse.is_a("IfcMaterialProfile"):
            for ms in inverse.ToMaterialProfileSet:
                results.update(_get_elements_by_material(ifc_file, ms))
        elif inverse.is_a("IfcMaterialConstituent"):
            for ms in inverse.ToMaterialConstituentSet:
                results.update(_get_elements_by_material(ifc_file, ms))
        elif inverse.is_a("IfcMaterialLayerSetUsage"):
            results.update(_get_elements_by_material(ifc_file, inverse))
        elif inverse.is_a("IfcMaterialProfileSetUsage"):
            results.update(_get_elements_by_material(ifc_file, inverse))
        elif inverse.is_a("IfcMaterialList"):
            results.update(_get_elements_by_material(ifc_file, inverse))
    return results


def _get_shape_aspects(ifc_file, element):
    representation = getattr(element, "Representation", None)
    if representation is None:
        return []
    shape_aspects = list(representation.HasShapeAspects or [])
    return shape_aspects
