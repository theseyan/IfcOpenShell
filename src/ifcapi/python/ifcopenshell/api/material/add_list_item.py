# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_list_item(file, material_list, material):
    materials = list(material_list.Materials or [])
    materials.append(material)
    material_list.Materials = materials
