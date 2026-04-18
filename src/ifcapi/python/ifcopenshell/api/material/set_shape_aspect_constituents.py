# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.material
import ifcopenshell.api.style
import ifcopenshell.util.element
import ifcopenshell.util.representation


def set_shape_aspect_constituents(file, *, element, context, materials):
    should_create_new_material_set = False
    if material := ifcopenshell.util.element.get_material(element):
        if (
            material.is_a("IfcMaterialConstituent")
            and len(names := [c.Name for c in material.MaterialConstituents]) == len(materials)
            and set(names) == set(materials.keys())
        ):
            should_create_new_material_set = False
        else:
            should_create_new_material_set = True
            ifcopenshell.api.material.unassign_material(file, products=[element])
            if not material.is_a("IfcMaterial") and not file.get_total_inverses(material):
                ifcopenshell.api.material.remove_material_set(file, material=material)
    else:
        should_create_new_material_set = True
    if should_create_new_material_set:
        material_set = ifcopenshell.api.material.add_material_set(file, set_type="IfcMaterialConstituentSet")
        for name, material in materials.items():
            ifcopenshell.api.material.add_constituent(file, constituent_set=material_set, material=material, name=name)
        ifcopenshell.api.material.assign_material(file, products=[element], material=material_set)

    styles = {n: ifcopenshell.util.representation.get_material_style(m, context) for n, m in materials.items()}
    representation = ifcopenshell.util.representation.get_representation(element, context=context)
    representation = ifcopenshell.util.representation.resolve_representation(representation)
    for item in representation.Items:
        if aspect := ifcopenshell.util.representation.get_item_shape_aspect(representation, item):
            if style := styles.get(aspect.Name, None):
                ifcopenshell.api.style.assign_item_style(file, item=item, style=style)
