# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_constraint(file, products, constraint):
    if not products:
        return
    products_set = set(products)

    rels = [rel for rel in file.get_inverse(constraint) if rel.is_a("IfcRelAssociatesConstraint")]
    related_objects = set()
    for rel in rels:
        related_objects.update(rel.RelatedObjects)

    products_to_assign = products_set - related_objects
    if not products_to_assign:
        return rels[0] if rels else None

    rel = next(iter(rels), None)
    if rel:
        related_objects = set(rel.RelatedObjects) | products_to_assign
        rel.RelatedObjects = list(related_objects)
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
        return rel

    return file.create_entity(
        "IfcRelAssociatesConstraint",
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        RelatingConstraint=constraint,
        RelatedObjects=list(products_to_assign),
    )
