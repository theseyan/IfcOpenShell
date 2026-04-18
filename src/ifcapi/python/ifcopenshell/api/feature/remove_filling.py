# SPDX-License-Identifier: LGPL-3.0-or-later

"""Remove a filling relationship."""

import ifcopenshell.util.element


def remove_filling(file, element=None):
    """Remove the IfcRelFillsElement relationship for the given element."""
    for rel in file.by_type("IfcRelFillsElement"):
        if rel.RelatedBuildingElement == element:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
            break
