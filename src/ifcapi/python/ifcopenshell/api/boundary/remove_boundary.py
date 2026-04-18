# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_boundary(file, boundary=None):
    geometry = boundary.ConnectionGeometry
    history = boundary.OwnerHistory
    file.remove(boundary)
    if geometry:
        ifcopenshell.util.element.remove_deep2(file, geometry)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
