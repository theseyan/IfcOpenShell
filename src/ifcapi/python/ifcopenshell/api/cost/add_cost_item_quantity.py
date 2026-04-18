# SPDX-License-Identifier: LGPL-3.0-or-later


def add_cost_item_quantity(file, cost_item=None, ifc_class="IfcQuantityCount"):
    quantity = file.create_entity(ifc_class, Name="Unnamed")
    if ifc_class == "IfcQuantityCount":
        count = 0
        for rel in cost_item.Controls:
            count += len(rel.RelatedObjects)
        quantity[3] = count
    else:
        quantity[3] = 0.0
    quantities = list(cost_item.CostQuantities or [])
    quantities.append(quantity)
    cost_item.CostQuantities = quantities
    return quantity
