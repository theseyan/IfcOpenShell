# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def assign_reference(file, products, reference):
    referenced_elements = ifcopenshell.util.element.get_referenced_elements(reference)
    products_set = set(products) - referenced_elements
    if not products_set:
        return

    if file.schema == "IFC2X3":
        rel = next(
            (r for r in file.by_type("IfcRelAssociatesLibrary") if r.RelatingLibrary == reference),
            None,
        )
    else:
        rel = next(iter(reference.LibraryRefForObjects), None)

    if not rel:
        return file.create_entity(
            "IfcRelAssociatesLibrary",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=list(products_set),
            RelatingLibrary=reference,
        )

    related_objects = set(rel.RelatedObjects) | products_set
    rel.RelatedObjects = list(related_objects)
    ifcopenshell.api.owner.update_owner_history(file, element=rel)
    return rel
