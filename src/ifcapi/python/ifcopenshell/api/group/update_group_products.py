# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def update_group_products(file, group, products):
    if not group.IsGroupedBy:
        return file.create_entity(
            "IfcRelAssignsToGroup",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedObjects": products,
                "RelatingGroup": group,
            }
        )
    else:
        rels = group.IsGroupedBy
        objects = set(products)
        for rel in rels:
            objects.update([g for g in rel.RelatedObjects if g.is_a("IfcGroup")])
        to_purge = rels[1:]

        for rel in to_purge:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

        rels[0].RelatedObjects = list(objects)
        return rels[0]
