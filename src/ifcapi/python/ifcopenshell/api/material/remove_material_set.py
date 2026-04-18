# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.material
import ifcopenshell.util.element


def remove_material_set(file, material):
    has_usages = material.is_a("IfcMaterialLayerSet") or material.is_a("IfcMaterialProfileSet")
    if has_usages:
        elements = ifcopenshell.util.element.get_elements_by_material(file, material)
        if elements:
            ifcopenshell.api.material.unassign_material(file, products=list(elements))

    if material.is_a("IfcMaterialLayerSet"):
        set_items = material.MaterialLayers or []
    elif material.is_a("IfcMaterialProfileSet"):
        set_items = material.MaterialProfiles or []
    elif material.is_a("IfcMaterialConstituentSet"):
        set_items = material.MaterialConstituents or []
    elif material.is_a("IfcMaterialList"):
        set_items = []
    else:
        raise ValueError(f"Unknown material set type: {material.is_a()}")
    for set_item in set_items:
        file.remove(set_item)

    inverse_elements = file.get_inverse(material)
    file.remove(material)

    for inverse in inverse_elements:
        if inverse.is_a("IfcRelAssociatesMaterial"):
            history = inverse.OwnerHistory
            file.remove(inverse)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
        elif inverse.is_a("IfcMaterialProperties"):
            for prop in inverse.Properties or []:
                file.remove(prop)
            file.remove(inverse)
