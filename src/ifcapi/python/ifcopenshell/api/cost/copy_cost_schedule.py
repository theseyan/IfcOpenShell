# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.control
import ifcopenshell.api.cost
import ifcopenshell.util.element


def copy_cost_schedule(file, cost_schedule=None):
    new_schedule = ifcopenshell.util.element.copy(file, cost_schedule)

    for rel in cost_schedule.Controls:
        for cost_item in rel.RelatedObjects:
            duplicated_cost_item = ifcopenshell.api.cost.copy_cost_item(file, cost_item=cost_item)
            if isinstance(duplicated_cost_item, list):
                duplicated_cost_item = duplicated_cost_item[0]
            ifcopenshell.api.control.assign_control(file, new_schedule, [duplicated_cost_item])
    return new_schedule
