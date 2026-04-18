# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.aggregate
import ifcopenshell.api.geometry
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element
import ifcopenshell.util.placement


def assign_container(file, products, relating_structure):
    if not products:
        return

    products_set = set(products)
    structure_rel = next(iter(relating_structure.ContainsElements), None)

    previous_containers_rels = set()
    products_without_containers = []
    products_with_containers = []

    # check if there is anything to change
    for product in products_set:
        product_rel = next(iter(product.ContainedInStructure), None)

        if product_rel is None:
            products_without_containers.append(product)
            continue

        # either structure_rel is None or product is part of different rel
        if product_rel != structure_rel:
            previous_containers_rels.add(product_rel)
            products_with_containers.append(product)

        # products with already assigned containers will be skipped

    products_to_change = products_without_containers + products_with_containers
    # nothing to change
    if not products_to_change:
        return structure_rel

    # can be either only aggregated or only contained at the same time
    ifcopenshell.api.aggregate.unassign_object(file, products=products_without_containers)

    # unassign elements from previous containers
    for rel in previous_containers_rels:
        related_elements = set(rel.RelatedElements) - products_set
        if related_elements:
            rel.RelatedElements = list(related_elements)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    # assign elements to a new container
    if structure_rel:
        structure_rel.RelatedElements = list(set(structure_rel.RelatedElements) | products_set)
        ifcopenshell.api.owner.update_owner_history(file, element=structure_rel)
    else:
        structure_rel = file.create_entity(
            "IfcRelContainedInSpatialStructure",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedElements": list(products_set),
                "RelatingStructure": relating_structure,
            }
        )

    # localize placement relative to a new container for affected products
    for product in products_to_change:
        placement = getattr(product, "ObjectPlacement", None)
        if placement and placement.is_a("IfcLocalPlacement"):
            ifcopenshell.api.geometry.edit_object_placement(
                file,
                product=product,
                matrix=ifcopenshell.util.placement.get_local_placement(product.ObjectPlacement),
                is_si=False,
            )

    return structure_rel
