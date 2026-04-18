# SPDX-License-Identifier: LGPL-3.0-or-later

"""Unassign a property set from products without deleting it."""

import ifcopenshell
import ifcopenshell.util.element


def unassign_pset(file, products, pset):
    """Remove the *pset* assignment from each element in *products*."""
    is_ifc2x3 = file.schema == "IFC2X3"

    products_occurrences = set()
    products_types = set()
    for product in products:
        if product.is_a("IfcTypeProduct"):
            products_types.add(product)
        else:
            products_occurrences.add(product)

    if products_occurrences:
        rels = getattr(pset, "PropertyDefinitionOf", ()) if is_ifc2x3 else getattr(pset, "DefinesOccurrence", ())
        for rel in rels or ():
            objs = set(rel.RelatedObjects)
            if not any(p in objs for p in products_occurrences):
                continue
            objs.difference_update(products_occurrences)
            if objs:
                rel.RelatedObjects = list(objs)
            else:
                history = getattr(rel, "OwnerHistory", None)
                file.remove(rel)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)

    for product in products_types:
        psets = list(getattr(product, "HasPropertySets", ()) or ())
        if pset in psets:
            psets.remove(pset)
        product.HasPropertySets = psets or None
