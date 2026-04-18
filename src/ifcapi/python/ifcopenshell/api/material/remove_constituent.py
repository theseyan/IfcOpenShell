# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_constituent(file, constituent, should_remove_material=False):
    material = constituent.Material
    file.remove(constituent)
    if material and should_remove_material:
        ifcopenshell.util.element.remove_deep2(file, material)
