# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def dereference_structure(file, products, relating_structure):
    products_set = set(products)
    for rel in relating_structure.ReferencesElements:
        related_elements = set(rel.RelatedElements)
        if not related_elements.intersection(products_set):
            continue
        related_elements = related_elements - products_set
        if related_elements:
            rel.RelatedElements = list(related_elements)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
