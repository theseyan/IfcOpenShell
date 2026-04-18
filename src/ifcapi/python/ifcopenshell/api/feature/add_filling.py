# SPDX-License-Identifier: LGPL-3.0-or-later

"""Fill an opening with an element."""

import ifcopenshell
import ifcopenshell.guid
import ifcopenshell.util.element


def add_filling(file, opening=None, element=None):
    """Create an IfcRelFillsElement between an opening and an element."""
    fills_voids = element.FillsVoids

    if fills_voids:
        if fills_voids[0].RelatingOpeningElement == opening:
            return fills_voids[0]
        history = fills_voids[0].OwnerHistory
        file.remove(fills_voids[0])
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)

    return file.create_entity(
        "IfcRelFillsElement",
        GlobalId=ifcopenshell.guid.new(),
        RelatingOpeningElement=opening,
        RelatedBuildingElement=element,
    )
