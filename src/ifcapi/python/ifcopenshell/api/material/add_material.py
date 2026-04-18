# SPDX-License-Identifier: LGPL-3.0-or-later

"""material.add_material — create an IfcMaterial entity."""

import ifcopenshell


def add_material(file, name=None, category=None, description=None):
    material = file.create_entity("IfcMaterial", **{"Name": name or "Unnamed"})
    if category:
        material.Category = category
    if description:
        material.Description = description
    return material
