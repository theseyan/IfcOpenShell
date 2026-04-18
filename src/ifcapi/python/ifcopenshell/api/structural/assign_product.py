# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def assign_product(file, relating_product=None, related_object=None):
    for rel in relating_product.ReferencedBy or []:
        if not rel.is_a("IfcRelAssignsToProduct"):
            continue
        if related_object in rel.RelatedObjects:
            return rel
        related_objects = list(rel.RelatedObjects)
        related_objects.append(related_object)
        rel.RelatedObjects = related_objects
        return rel

    rel = ifcopenshell.api.root.create_entity(file, ifc_class="IfcRelAssignsToProduct")
    rel.RelatingProduct = relating_product
    rel.RelatedObjects = [related_object]
    return rel
