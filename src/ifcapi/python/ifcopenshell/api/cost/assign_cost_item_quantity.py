# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.control
import ifcopenshell.api.cost


def assign_cost_item_quantity(file, cost_item=None, products=None, prop_name=""):
    products = products or []
    if prop_name:
        quantities = set(cost_item.CostQuantities or [])

    for product in products:
        if product.is_a("IfcSpatialElement"):
            continue
        ifcopenshell.api.control.assign_control(
            file, related_objects=[product], relating_control=cost_item
        )
        if prop_name:
            if (
                cost_item.CostQuantities
                and cost_item.CostQuantities[0].Name.lower() != prop_name.lower()
            ):
                continue
            _add_quantity_from_related_object(product, prop_name, quantities)

    if prop_name:
        cost_item.CostQuantities = list(quantities)
    else:
        _update_cost_item_count(file, cost_item)


def _add_quantity_from_related_object(element, prop_name, quantities):
    for relationship in element.IsDefinedBy:
        if relationship.is_a("IfcRelDefinesByProperties"):
            _add_quantity_from_qto(relationship.RelatingPropertyDefinition, prop_name, quantities)


def _add_quantity_from_qto(qto, prop_name, quantities):
    if not qto.is_a("IfcElementQuantity"):
        return
    for prop in qto.Quantities:
        if prop.is_a("IfcPhysicalSimpleQuantity") and prop.Name.lower() == prop_name.lower():
            quantities.add(prop)


def _update_cost_item_count(file, cost_item):
    if not cost_item.CostQuantities:
        ifcopenshell.api.cost.add_cost_item_quantity(
            file, cost_item=cost_item, ifc_class="IfcQuantityCount"
        )
    if len(cost_item.CostQuantities) == 1:
        quantity = cost_item.CostQuantities[0]
        if quantity.is_a("IfcQuantityCount"):
            count = 0
            for rel in cost_item.Controls:
                for obj in rel.RelatedObjects:
                    if not obj.is_a("IfcConstructionResource"):
                        count += 1
            quantity[3] = count
