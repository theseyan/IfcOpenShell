# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def copy_boundary(file, boundary=None):
    result = ifcopenshell.util.element.copy(file, boundary)
    if result.ConnectionGeometry:
        result.ConnectionGeometry = ifcopenshell.util.element.copy_deep(file, result.ConnectionGeometry)
    return result
