# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.cost


def assign_cost_value(file, cost_item=None, cost_rate=None):
    if cost_item.CostValues:
        for cost_value in list(cost_item.CostValues):
            ifcopenshell.api.cost.remove_cost_value(
                file, parent=cost_item, cost_value=cost_value
            )
    # This is an assumption, and not part of the official IFC documentation
    cost_item.CostValues = cost_rate.CostValues
