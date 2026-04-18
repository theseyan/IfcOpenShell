# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def copy_material(file, material):
    if material.is_a("IfcMaterial"):
        return _copy_material_with_inverses(file, material)
    elif material.is_a("IfcMaterialConstituentSet"):
        new = _copy_material_with_inverses(file, material)
        new.MaterialConstituents = [copy_material(file, i) for i in material.MaterialConstituents]
        return new
    elif material.is_a("IfcMaterialConstituent"):
        return _copy_material_with_inverses(file, material)
    elif material.is_a("IfcMaterialLayerSet"):
        new = _copy_material_with_inverses(file, material)
        new.MaterialLayers = [copy_material(file, i) for i in material.MaterialLayers]
        return new
    elif material.is_a("IfcMaterialLayer"):
        return _copy_material_with_inverses(file, material)
    elif material.is_a("IfcMaterialProfileSet"):
        new = _copy_material_with_inverses(file, material)
        new.MaterialProfiles = [copy_material(file, i) for i in material.MaterialProfiles]
        return new
    elif material.is_a("IfcMaterialProfile"):
        return _copy_material_with_inverses(file, material)
    elif material.is_a("IfcMaterialList"):
        return _copy_material_with_inverses(file, material)
    else:
        raise Exception(f"Unexpected material type: '{material.is_a()}' ({material}).")


def _copy_material_with_inverses(file, material):
    new = ifcopenshell.util.element.copy(file, material)
    for inverse in file.get_inverse(material):
        if inverse.is_a("IfcMaterialProperties"):
            inverse = ifcopenshell.util.element.copy(file, inverse)
            inverse.Material = new

            props_attribute = "Properties"
            if file.schema == "IFC2X3":
                if not inverse.is_a("IfcExtendedMaterialProperties"):
                    continue
                props_attribute = "ExtendedProperties"

            props = getattr(inverse, props_attribute)
            if not props:
                continue

            copied_props = []
            for pset in props:
                copied_props.append(ifcopenshell.util.element.copy_deep(file, pset))
            setattr(inverse, props_attribute, copied_props)

        elif inverse.is_a("IfcMaterialDefinitionRepresentation"):
            inverse = ifcopenshell.util.element.copy_deep(
                file, inverse, exclude=["IfcRepresentationContext", "IfcMaterial", "IfcPresentationStyle"]
            )
            inverse.RepresentedMaterial = new
    return new
