# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_constraint(file, products, constraint):
    if not products:
        return
    products_set = set(products)

    rels = [rel for rel in file.get_inverse(constraint) if rel.is_a("IfcRelAssociatesConstraint")]
    for rel in rels:
        related_objects = set(rel.RelatedObjects)
        if not related_objects.intersection(products_set):
            continue
        related_objects -= products_set
        if related_objects:
            rel.RelatedObjects = list(related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
            continue
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
