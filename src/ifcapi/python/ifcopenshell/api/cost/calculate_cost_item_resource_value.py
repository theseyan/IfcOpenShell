# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.cost


def calculate_cost_item_resource_value(file, cost_item=None):
    """Calculate cost from controlled resources. Requires util.resource (not yet ported)."""
    for cost_value in cost_item.CostValues or []:
        ifcopenshell.api.cost.remove_cost_value(file, parent=cost_item, cost_value=cost_value)

    resources = []
    for rel in cost_item.Controls or []:
        for related_object in rel.RelatedObjects:
            if related_object.is_a("IfcConstructionResource"):
                resources.append(related_object)
            elif related_object.is_a("IfcTask"):
                for rel2 in related_object.OperatesOn or []:
                    for related_object2 in rel2.RelatedObjects:
                        if related_object2.is_a("IfcConstructionResource"):
                            resources.append(related_object2)

    for resource in resources:
        cost, unit = _get_resource_cost(resource)
        if cost is None:
            parent_cost = _get_parent_resource_cost(resource)
            if parent_cost:
                cost, unit = parent_cost
        quantity = _get_resource_quantity(resource)
        if cost is None:
            continue
        if unit and "day" in unit:
            quantity = quantity / 8  # Assume 8 hour working day
        formula = "{}*{}".format(cost, quantity)
        cost_value = ifcopenshell.api.cost.add_cost_value(file, parent=cost_item)
        cost_value.Name = resource.Name
        ifcopenshell.api.cost.edit_cost_value_formula(file, cost_value=cost_value, formula=formula)


def _get_resource_cost(resource):
    """Inline implementation of util.resource.get_cost."""
    total_cost = 0.0
    unit = None
    has_cost = False
    for cost_value in resource.BaseCosts or []:
        applied = cost_value.AppliedValue
        if applied is None:
            continue
        has_cost = True
        if hasattr(applied, "wrappedValue"):
            total_cost += applied.wrappedValue
        elif isinstance(applied, (int, float)):
            total_cost += applied
    if not has_cost:
        return None, None
    return total_cost, unit


def _get_parent_resource_cost(resource):
    """Check parent resource for inherited cost."""
    nests = getattr(resource, "Nests", None)
    if not nests:
        return None
    parent = nests[0].RelatingObject
    return _get_resource_cost(parent)


def _get_resource_quantity(resource):
    """Get resource base quantity value."""
    base_quantity = getattr(resource, "BaseQuantity", None)
    if base_quantity:
        return base_quantity[3]
    return 1.0
