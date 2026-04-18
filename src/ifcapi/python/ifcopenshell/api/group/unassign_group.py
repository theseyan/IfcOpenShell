# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_group(file, products, group):
    if not group.IsGroupedBy:
        return
    rel = group.IsGroupedBy[0]
    related_objects = set(rel.RelatedObjects) or set()
    products_set = set(products)
    related_objects -= products_set
    if related_objects:
        rel.RelatedObjects = list(related_objects)
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
    else:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
