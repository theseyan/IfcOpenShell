# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_structural_load_group(file, load_group=None):
    for inverse in file.get_inverse(load_group):
        if inverse.is_a("IfcRelAssignsToGroup") and len(inverse.RelatedObjects) == 1:
            history = inverse.OwnerHistory
            file.remove(inverse)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    history = load_group.OwnerHistory
    file.remove(load_group)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
