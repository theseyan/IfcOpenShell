# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.control


def unassign_cost_item_quantity(file, cost_item=None, products=None):
    products = products or []
    quantities = set(cost_item.CostQuantities or [])
    for quantity in cost_item.CostQuantities or []:
        for inverse in file.get_inverse(quantity):
            if not inverse.is_a("IfcElementQuantity"):
                continue
            for rel in inverse.DefinesOccurrence or []:
                for related_object in rel.RelatedObjects:
                    if related_object in products:
                        quantities.discard(quantity)
    cost_item.CostQuantities = list(quantities)
    for product in products:
        ifcopenshell.api.control.unassign_control(
            file, related_objects=[product], relating_control=cost_item
        )
    _update_cost_item_count(file, cost_item)


def _update_cost_item_count(file, cost_item):
    if not (cost_item.CostQuantities and len(cost_item.CostQuantities) == 1):
        return
    quantity = cost_item.CostQuantities[0]
    if quantity.is_a("IfcQuantityCount"):
        count = 0
        for rel in cost_item.Controls:
            count += len(rel.RelatedObjects)
        if count:
            quantity[3] = count
        else:
            file.remove(quantity)
