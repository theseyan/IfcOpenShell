# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""classification.add_reference — add a classification reference to products."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def add_reference(file, products, reference=None, identification=None,
                  name=None, classification=None, is_lightweight=True):
    if not products:
        return None

    rooted = set()
    non_rooted = set()
    for p in products:
        (rooted if p.is_a("IfcRoot") else non_rooted).add(p)

    if non_rooted and file.schema == "IFC2X3":
        raise TypeError("Cannot add reference to non-IfcRoot element in IFC2X3")

    if reference:
        raise NotImplementedError("Library reference import not yet supported")

    ref = _get_existing_reference(file, identification)
    if not ref:
        ref = file.createIfcClassificationReference(
            Name=name, ReferencedSource=classification)
        if file.schema == "IFC2X3":
            ref.ItemReference = identification
        else:
            ref.Identification = identification

    if rooted:
        _assign_to_rooted(file, ref, rooted)
    if non_rooted:
        _assign_to_non_rooted(file, ref, non_rooted)
    return ref


def _get_existing_reference(file, identification):
    for ref in file.by_type("IfcClassificationReference"):
        if file.schema == "IFC2X3":
            if ref.ItemReference == identification:
                return ref
        else:
            if ref.Identification == identification:
                return ref
    return None


def _assign_to_rooted(file, reference, products):
    rel = None
    if file.schema == "IFC2X3":
        for r in file.by_type("IfcRelAssociatesClassification"):
            if r.RelatingClassification == reference:
                rel = r
                break
    else:
        refs = getattr(reference, "ClassificationRefForObjects", ())
        rel = next(iter(refs or ()), None)

    if rel:
        objs = set(rel.RelatedObjects) | products
        rel.RelatedObjects = list(objs)
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
    else:
        file.create_entity("IfcRelAssociatesClassification",
                           GlobalId=ifcopenshell.guid.new(),
                           OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
                           RelatedObjects=list(products),
                           RelatingClassification=reference)


def _assign_to_non_rooted(file, reference, products):
    refs = getattr(reference, "ExternalReferenceForResources", ())
    rel = next(iter(refs or ()), None)
    if rel:
        objs = set(rel.RelatedResourceObjects) | products
        rel.RelatedResourceObjects = list(objs)
    else:
        file.create_entity("IfcExternalReferenceRelationship",
                           RelatingReference=reference,
                           RelatedResourceObjects=list(products))
