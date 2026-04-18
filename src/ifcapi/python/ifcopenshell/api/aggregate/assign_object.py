# SPDX-License-Identifier: LGPL-3.0-or-later

"""aggregate.assign_object — assign products as parts of an aggregate."""

import ifcopenshell
import ifcopenshell.api.geometry
import ifcopenshell.api.owner
import ifcopenshell.api.spatial
import ifcopenshell.guid
import ifcopenshell.util.element
import ifcopenshell.util.placement


def assign_object(file, products, relating_object):
    if not products:
        return

    products_set = set(products)
    is_decomposed_by = next((i for i in relating_object.IsDecomposedBy if i.is_a("IfcRelAggregates")), None)

    previous_aggregates_rels = set()
    products_without_aggregates = []
    products_with_aggregates = []

    for product in products_set:
        product_rel = next(iter(product.Decomposes), None)

        if product_rel is None:
            products_without_aggregates.append(product)
            continue

        if product_rel != is_decomposed_by:
            previous_aggregates_rels.add(product_rel)
            products_with_aggregates.append(product)

    products_to_change = products_without_aggregates + products_with_aggregates
    if not products_to_change:
        return is_decomposed_by

    possibly_contained_products = [p for p in products_without_aggregates if hasattr(p, "ContainedInStructure")]
    ifcopenshell.api.spatial.unassign_container(file, products=possibly_contained_products)

    for decomposes in previous_aggregates_rels:
        related_objects = set(decomposes.RelatedObjects) - products_set
        if related_objects:
            decomposes.RelatedObjects = list(related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=decomposes)
        else:
            history = decomposes.OwnerHistory
            file.remove(decomposes)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    if is_decomposed_by:
        is_decomposed_by.RelatedObjects = list(set(is_decomposed_by.RelatedObjects) | products_set)
        ifcopenshell.api.owner.update_owner_history(file, element=is_decomposed_by)
    else:
        is_decomposed_by = file.create_entity(
            "IfcRelAggregates",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedObjects": list(products_set),
                "RelatingObject": relating_object,
            }
        )

    for product in products_to_change:
        placement = getattr(product, "ObjectPlacement", None)
        if placement and placement.is_a("IfcLocalPlacement"):
            ifcopenshell.api.geometry.edit_object_placement(
                file,
                product=product,
                matrix=ifcopenshell.util.placement.get_local_placement(product.ObjectPlacement),
                is_si=False,
            )

    return is_decomposed_by
