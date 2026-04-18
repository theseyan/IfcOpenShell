# SPDX-License-Identifier: LGPL-3.0-or-later


def add_cost_value(file, parent=None):
    value = file.create_entity("IfcCostValue")
    if parent.is_a("IfcCostItem"):
        values = list(parent.CostValues or [])
        values.append(value)
        parent.CostValues = values
    elif parent.is_a("IfcConstructionResource"):
        values = list(parent.BaseCosts or [])
        values.append(value)
        parent.BaseCosts = values
    elif parent.is_a("IfcCostValue"):
        values = list(parent.Components or [])
        values.append(value)
        parent.Components = values
    return value
