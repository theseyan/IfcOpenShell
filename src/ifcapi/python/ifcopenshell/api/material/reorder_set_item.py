# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def reorder_set_item(file, material_set, old_index=0, new_index=0):
    if material_set.is_a("IfcMaterialConstituentSet"):
        set_name = "MaterialConstituents"
    elif material_set.is_a("IfcMaterialLayerSet"):
        set_name = "MaterialLayers"
    elif material_set.is_a("IfcMaterialProfileSet"):
        set_name = "MaterialProfiles"
    elif material_set.is_a("IfcMaterialList"):
        set_name = "Materials"
    else:
        raise ValueError(f"Unexpected material set type: '{material_set.is_a()}'.")

    items = list(getattr(material_set, set_name) or [])
    items.insert(new_index, items.pop(old_index))
    setattr(material_set, set_name, items)
