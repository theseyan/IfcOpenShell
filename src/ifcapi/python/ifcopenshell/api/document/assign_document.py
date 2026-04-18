# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.assign_document — assign a document reference to products."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def assign_document(file, products, document):
    referenced_elements = ifcopenshell.util.element.get_referenced_elements(document)
    products_set = set(products) - referenced_elements
    if not products_set:
        return

    if file.schema == "IFC2X3":
        rel = next(
            (r for r in file.by_type("IfcRelAssociatesDocument") if r.RelatingDocument == document),
            None,
        )
    else:
        ifc_class = document.is_a()
        if ifc_class == "IfcDocumentReference":
            rel = next(iter(document.DocumentRefForObjects), None)
        elif ifc_class == "IfcDocumentInformation":
            rel = next(iter(document.DocumentInfoForObjects), None)
        else:
            rel = None

    if not rel:
        return file.create_entity(
            "IfcRelAssociatesDocument",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=list(products_set),
            RelatingDocument=document,
        )

    related_objects = set(rel.RelatedObjects) | products_set
    rel.RelatedObjects = list(related_objects)
    ifcopenshell.api.owner.update_owner_history(file, element=rel)
    return rel
