# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.util.element


def unshare_pset(file, products, pset):
    if not products:
        raise Exception("No products provided.")

    pset_elements = ifcopenshell.util.element.get_elements_by_pset(pset)
    products_original = products

    if set(products) == pset_elements:
        products = products[1:]

    if not products:
        raise Exception(f"Provided product is the only element to which pset is assigned: {products_original[0]}.")

    products_occurrences = set()
    products_types = set()
    for product in products:
        if product.is_a("IfcTypeProduct"):
            products_types.add(product)
        else:
            products_occurrences.add(product)

    ifcopenshell.api.pset.unassign_pset(file, products, pset)

    pset_copies = []
    for product in products:
        pset_copy = ifcopenshell.util.element.copy(file, pset)
        pset_copies.append(pset_copy)
        ifcopenshell.api.pset.assign_pset(file, [product], pset_copy)

    return pset_copies
