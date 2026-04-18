# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def remove_cost_value(file, parent=None, cost_value=None):
    if file.get_total_inverses(cost_value) == 1:
        file.remove(cost_value)
    elif parent.is_a("IfcCostItem"):
        values = list(parent.CostValues)
        values.remove(cost_value)
        parent.CostValues = values if values else None
    elif parent.is_a("IfcConstructionResource"):
        values = list(parent.BaseCosts)
        values.remove(cost_value)
        parent.BaseCosts = values if values else None
    elif parent.is_a("IfcCostValue"):
        components = list(parent.Components)
        components.remove(cost_value)
        parent.Components = components if components else None
