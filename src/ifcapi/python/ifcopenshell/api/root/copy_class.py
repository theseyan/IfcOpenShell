# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.geometry
import ifcopenshell.api.material
import ifcopenshell.api.root
import ifcopenshell.api.system
import ifcopenshell.util.element
import ifcopenshell.util.placement


def copy_class(file, product):
    result = ifcopenshell.util.element.copy(file, product)
    _copy_direct_attributes(file, result)
    _copy_indirect_attributes(file, product, result)
    return result


def _copy_direct_attributes(file, to_element):
    _remove_representations(to_element)
    _copy_object_placements(file, to_element)
    _copy_psets(file, to_element)


def _copy_indirect_attributes(file, from_element, to_element):
    for inverse in file.get_inverse(from_element):
        if inverse.is_a("IfcRelDefinesByProperties"):
            inverse = ifcopenshell.util.element.copy(file, inverse)
            inverse.RelatedObjects = [to_element]
            pset = ifcopenshell.util.element.copy_deep(file, inverse.RelatingPropertyDefinition)
            inverse.RelatingPropertyDefinition = pset
        elif (
            inverse.is_a("IfcRelNests")
            and inverse.RelatingObject == from_element
            or inverse.is_a("IfcRelConnectsPortToElement")
            and inverse.RelatedElement == from_element
        ):
            if inverse.is_a("IfcRelNests"):
                ports = [e for e in inverse.RelatedObjects if e.is_a("IfcDistributionPort")]
            else:
                ports = [inverse.RelatingPort]
            if not ports:
                continue
            new_ports = [ifcopenshell.api.root.copy_class(file, product=p) for p in ports]
            inverse = ifcopenshell.util.element.copy(file, inverse)

            if inverse.is_a("IfcRelNests"):
                inverse.RelatingObject = to_element
                inverse.RelatedObjects = new_ports
            else:
                inverse.RelatedElement = to_element
                inverse.RelatingPort = new_ports[0]

            for port in new_ports:
                ifcopenshell.api.system.unassign_port(file, element=from_element, port=port)
                ifcopenshell.api.system.disconnect_port(file, port=port)
                matrix = ifcopenshell.util.placement.get_local_placement(port.ObjectPlacement)
                ifcopenshell.api.geometry.edit_object_placement(
                    file,
                    product=port,
                    matrix=matrix,
                    is_si=False,
                    should_transform_children=False,
                )
        elif inverse.is_a("IfcRelAggregates") and inverse.RelatingObject == from_element:
            continue
        elif inverse.is_a("IfcRelContainedInSpatialStructure") and inverse.RelatingStructure == from_element:
            continue
        elif inverse.is_a("IfcRelDefinesByType") and inverse.RelatingType == from_element:
            continue
        elif inverse.is_a("IfcRelVoidsElement") and inverse.RelatingBuildingElement == from_element:
            opening = inverse.RelatedOpeningElement
            if not opening.is_a("IfcOpeningElement") or not opening.HasFillings:
                new_opening = ifcopenshell.api.root.copy_class(file, product=opening)
                new_opening.VoidsElements[0].RelatingBuildingElement = to_element
                if new_opening.ObjectPlacement and new_opening.ObjectPlacement.is_a("IfcLocalPlacement"):
                    if to_element.ObjectPlacement:
                        new_opening.ObjectPlacement.PlacementRelTo = to_element.ObjectPlacement
                if opening.Representation:
                    new_opening.Representation = ifcopenshell.util.element.copy_deep(
                        file, opening.Representation, exclude=["IfcGeometricRepresentationContext"]
                    )
        elif inverse.is_a("IfcRelFillsElement"):
            continue
        elif inverse.is_a("IfcRelConnectsPathElements"):
            continue
        elif inverse.is_a("IfcRelAssociatesMaterial") and "Usage" in inverse.RelatingMaterial.is_a():
            inverse = ifcopenshell.util.element.copy(file, inverse)
            inverse.RelatingMaterial = ifcopenshell.util.element.copy(file, inverse.RelatingMaterial)
            inverse.RelatedObjects = [to_element]
        elif inverse.is_a("IfcRelAssociatesMaterial") and "Set" in inverse.RelatingMaterial.is_a():
            inverse = ifcopenshell.util.element.copy(file, inverse)
            inverse.RelatingMaterial = ifcopenshell.api.material.copy_material(file, inverse.RelatingMaterial)
            inverse.RelatedObjects = [to_element]
        else:
            for i, value in enumerate(inverse):
                if value == from_element:
                    new_inverse = ifcopenshell.util.element.copy(file, inverse)
                    new_inverse[i] = to_element
                elif isinstance(value, (tuple, list)) and from_element in value:
                    new_value = list(value)
                    new_value.append(to_element)
                    inverse[i] = new_value


def _remove_representations(element):
    if element.is_a("IfcProduct"):
        element.Representation = None
    elif element.is_a("IfcTypeProduct"):
        element.RepresentationMaps = None


def _copy_object_placements(file, element):
    if not element.is_a("IfcProduct") or not element.ObjectPlacement:
        return
    element.ObjectPlacement = ifcopenshell.util.element.copy(file, element.ObjectPlacement)
    element.ObjectPlacement.RelativePlacement = ifcopenshell.util.element.copy_deep(
        file, element.ObjectPlacement.RelativePlacement
    )


def _copy_psets(file, element):
    if not element.is_a("IfcTypeObject") or not element.HasPropertySets:
        return
    element.HasPropertySets = [
        ifcopenshell.util.element.copy_deep(file, pset) for pset in element.HasPropertySets
    ]
