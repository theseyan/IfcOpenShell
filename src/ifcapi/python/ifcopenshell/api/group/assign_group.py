# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_group(file, products, group):
    if not products:
        return

    if not (is_grouped_by := group.IsGroupedBy):
        return file.create_entity(
            "IfcRelAssignsToGroup",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedObjects": products,
                "RelatingGroup": group,
            },
        )
    rel = is_grouped_by[0]
    related_objects = set(rel.RelatedObjects) or set()
    products_set = set(products)
    if products_set.issubset(related_objects):
        return rel
    rel.RelatedObjects = list(related_objects | products_set)
    ifcopenshell.api.owner.update_owner_history(file, element=rel)
    return rel
