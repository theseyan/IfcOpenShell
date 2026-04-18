# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.control
import ifcopenshell.api.nest
import ifcopenshell.api.root


def add_cost_item(file, cost_schedule=None, cost_item=None):
    cost_item_ = ifcopenshell.api.root.create_entity(file, ifc_class="IfcCostItem")

    if cost_schedule:
        ifcopenshell.api.control.assign_control(file, cost_schedule, [cost_item_])
    elif cost_item:
        ifcopenshell.api.nest.assign_object(file, related_objects=[cost_item_], relating_object=cost_item)
    return cost_item_
