# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.cost
import ifcopenshell.util.element


def remove_cost_schedule(file, cost_schedule=None):
    for inverse in file.get_inverse(cost_schedule):
        if inverse.is_a("IfcRelAssignsToControl"):
            for related_object in inverse.RelatedObjects:
                if related_object.is_a("IfcCostItem"):
                    ifcopenshell.api.cost.remove_cost_item(file, cost_item=related_object)
    history = cost_schedule.OwnerHistory
    file.remove(cost_schedule)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
