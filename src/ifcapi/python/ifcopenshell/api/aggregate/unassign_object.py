# SPDX-License-Identifier: LGPL-3.0-or-later

"""aggregate.unassign_object — unassign products from their aggregate."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_object(file, products):
    products = set(products)
    rels = set(
        rel
        for product in products
        if (rel := next((rel for rel in product.Decomposes if rel.is_a("IfcRelAggregates")), None))
    )

    for rel in rels:
        related_objects = set(rel.RelatedObjects) - products
        if related_objects:
            rel.RelatedObjects = list(related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
