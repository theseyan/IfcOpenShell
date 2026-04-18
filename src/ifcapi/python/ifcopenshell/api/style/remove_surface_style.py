# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_surface_style(file, style=None):
    to_delete = set()
    if style.is_a("IfcSurfaceStyleWithTextures"):
        textures = style.Textures
        if file.schema == "IFC2X3":
            to_delete.update(textures)
        else:
            for texture in textures:
                coords = texture.IsMappedBy
                if coords:
                    for coordinate in coords:
                        to_delete.add(coordinate)
                else:
                    to_delete.add(texture)

    for attribute in style:
        if isinstance(attribute, ifcopenshell.entity_instance) and attribute.id():
            to_delete.add(attribute)

    file.remove(style)

    for element in to_delete:
        ifcopenshell.util.element.remove_deep2(file, element)
