# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.cost
import ifcopenshell.util.element


def remove_cost_item(file, cost_item=None):
    for inverse in file.get_inverse(cost_item):
        if inverse.is_a("IfcRelNests"):
            if inverse.RelatingObject == cost_item:
                for related_object in inverse.RelatedObjects:
                    ifcopenshell.api.cost.remove_cost_item(file, cost_item=related_object)
            elif inverse.RelatedObjects == (cost_item,):
                history = inverse.OwnerHistory
                file.remove(inverse)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)
        elif inverse.is_a("IfcRelAssignsToControl"):
            if len(inverse.RelatedObjects) >= 2 or inverse.RelatingControl == cost_item:
                continue
            history = inverse.OwnerHistory
            file.remove(inverse)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    history = cost_item.OwnerHistory
    file.remove(cost_item)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
