# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_layer(file, layer, should_remove_material=False):
    material = layer.Material
    file.remove(layer)
    if material and should_remove_material:
        ifcopenshell.util.element.remove_deep2(file, material)
