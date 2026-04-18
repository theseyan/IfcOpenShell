# SPDX-License-Identifier: LGPL-3.0-or-later

"""classification.remove_reference — remove a classification reference from products."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def remove_reference(file, reference, products):
    products_set = set(products)
    if not products_set:
        return

    rooted = set()
    non_rooted = set()
    for p in products_set:
        (rooted if p.is_a("IfcRoot") else non_rooted).add(p)

    if non_rooted and file.schema == "IFC2X3":
        raise TypeError("Cannot remove reference from non-IfcRoot element in IFC2X3")

    if rooted:
        rels = set()
        for p in rooted:
            for a in getattr(p, "HasAssociations", ()) or ():
                if a.is_a("IfcRelAssociatesClassification") and a.RelatingClassification == reference:
                    rels.add(a)
        for rel in rels:
            remaining = set(rel.RelatedObjects) - rooted
            if remaining:
                rel.RelatedObjects = list(remaining)
                ifcopenshell.api.owner.update_owner_history(file, element=rel)
            else:
                history = rel.OwnerHistory
                file.remove(rel)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)

    if non_rooted:
        rels = set()
        for p in non_rooted:
            for r in getattr(p, "HasExternalReferences", ()) or getattr(p, "HasExternalReference", ()) or ():
                if r.RelatingReference == reference:
                    rels.add(r)
        for rel in rels:
            remaining = set(rel.RelatedResourceObjects) - non_rooted
            if remaining:
                rel.RelatedResourceObjects = list(remaining)
            else:
                file.remove(rel)

    # Check if reference is now orphaned
    ref_elements = _get_referenced_elements(file, reference)
    if not ref_elements:
        file.remove(reference)


def _get_referenced_elements(file, reference):
    elements = set()
    if file.schema == "IFC2X3":
        for rel in file.by_type("IfcRelAssociatesClassification"):
            if rel.RelatingClassification == reference:
                for obj in rel.RelatedObjects:
                    elements.add(obj)
    else:
        for rel in getattr(reference, "ClassificationRefForObjects", ()) or ():
            for obj in rel.RelatedObjects:
                elements.add(obj)
        for rel in getattr(reference, "ExternalReferenceForResources", ()) or ():
            for obj in rel.RelatedResourceObjects:
                elements.add(obj)
    return elements
