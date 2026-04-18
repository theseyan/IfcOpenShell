# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_product(file, relating_product=None, related_object=None):
    if relating_product.is_a("IfcGridAxis"):
        grid = None
        for attribute in ("PartOfW", "PartOfV", "PartOfU"):
            if getattr(relating_product, attribute, None):
                grid = getattr(relating_product, attribute)[0]
                break
        relating_product = grid

    for rel in related_object.HasAssignments or []:
        if not rel.is_a("IfcRelAssignsToProduct") or rel.RelatingProduct != relating_product:
            continue
        if len(rel.RelatedObjects) == 1:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
            return
        related_objects = list(rel.RelatedObjects)
        related_objects.remove(related_object)
        rel.RelatedObjects = related_objects
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
