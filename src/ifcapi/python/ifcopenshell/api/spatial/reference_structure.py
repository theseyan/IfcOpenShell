# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def reference_structure(file, products, relating_structure):
    structure = relating_structure
    products_set = set(products)

    if not products_set:
        return

    referenced = ifcopenshell.util.element.get_structure_referenced_elements(structure)
    products_to_assign = products_set - referenced
    rel = next(iter(structure.ReferencesElements), None)

    if not products_to_assign:
        return rel

    if rel is None:
        rel = file.create_entity(
            "IfcRelReferencedInSpatialStructure",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedElements=list(products_to_assign),
            RelatingStructure=structure,
        )
    else:
        related_elements = set(rel.RelatedElements) | products_to_assign
        rel.RelatedElements = list(related_elements)
        ifcopenshell.api.owner.update_owner_history(file, element=rel)

    return rel
