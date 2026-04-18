# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def remove_list_item(file, material_list, material_index=0):
    materials = list(material_list.Materials)
    materials.pop(material_index)
    material_list.Materials = materials
