# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_product(file, relating_product=None, related_object=None):
    if relating_product.is_a("IfcGridAxis"):
        axis = relating_product
        grid = None
        for attribute in ("PartOfW", "PartOfV", "PartOfU"):
            if getattr(axis, attribute, None):
                grid = getattr(axis, attribute)[0]
                break
        for rel in grid.ReferencedBy or []:
            if rel.Name == axis.AxisTag and related_object in rel.RelatedObjects:
                return
        return file.create_entity(
            "IfcRelAssignsToProduct",
            GlobalId=ifcopenshell.guid.new(),
            Name=axis.AxisTag,
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=[related_object],
            RelatingProduct=grid,
        )

    if related_object.HasAssignments:
        for rel in related_object.HasAssignments:
            if rel.is_a("IfcRelAssignsToProduct") and rel.RelatingProduct == relating_product:
                return

    referenced_by = None
    if relating_product.ReferencedBy:
        referenced_by = relating_product.ReferencedBy[0]

    if referenced_by:
        related_objects = list(referenced_by.RelatedObjects)
        related_objects.append(related_object)
        referenced_by.RelatedObjects = related_objects
        ifcopenshell.api.owner.update_owner_history(file, element=referenced_by)
    else:
        referenced_by = file.create_entity(
            "IfcRelAssignsToProduct",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=[related_object],
            RelatingProduct=relating_product,
        )

    return referenced_by
