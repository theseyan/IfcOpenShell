# SPDX-License-Identifier: LGPL-3.0-or-later

"""Assign a property set to one or more products."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_pset(file, products, pset):
    """Assign *pset* to each element in *products*.

    Returns the IfcRelDefinesByProperties if any occurrences are in *products*,
    or None if only type products are provided.
    """
    is_ifc2x3 = file.schema == "IFC2X3"

    products_occurrences = set()
    products_types = set()
    for product in products:
        if product.is_a("IfcTypeProduct"):
            products_types.add(product)
        else:
            products_occurrences.add(product)

    rel = None
    if products_occurrences:
        rels = getattr(pset, "PropertyDefinitionOf", ()) if is_ifc2x3 else getattr(pset, "DefinesOccurrence", ())
        rel = next(iter(rels or ()), None)
        if rel is not None:
            objs = set(rel.RelatedObjects) | products_occurrences
            rel.RelatedObjects = list(objs)
        else:
            rel = file.create_entity(
                "IfcRelDefinesByProperties",
                GlobalId=ifcopenshell.guid.new(),
                OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
                RelatedObjects=list(products_occurrences),
                RelatingPropertyDefinition=pset,
            )

    for product in products_types:
        psets = list(getattr(product, "HasPropertySets", ()) or ())
        product.HasPropertySets = psets + [pset]

    return rel
