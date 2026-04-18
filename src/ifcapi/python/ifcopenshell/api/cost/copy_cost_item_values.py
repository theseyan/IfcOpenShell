# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.cost
import ifcopenshell.util.element


def copy_cost_item_values(file, source=None, destination=None):
    for cost_value in destination.CostValues or []:
        ifcopenshell.api.cost.remove_cost_value(file, parent=source, cost_value=cost_value)
    copied_cost_values = []
    for cost_value in source.CostValues or []:
        copied_cost_values.append(ifcopenshell.util.element.copy_deep(file, cost_value))
    destination.CostValues = copied_cost_values
