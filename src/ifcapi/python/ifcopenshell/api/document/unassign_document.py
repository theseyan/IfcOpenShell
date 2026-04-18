# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.unassign_document — remove document association from products."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_document(file, products, document):
    reference_rels = set()
    products_set = set(products)
    for product in products_set:
        reference_rels.update(product.HasAssociations)

    reference_rels = {
        rel for rel in reference_rels
        if rel.is_a("IfcRelAssociatesDocument") and rel.RelatingDocument == document
    }

    for rel in reference_rels:
        related_objects = set(rel.RelatedObjects) - products_set
        if related_objects:
            rel.RelatedObjects = list(related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
