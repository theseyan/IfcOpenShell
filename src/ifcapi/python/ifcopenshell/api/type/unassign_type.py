# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_type(file, related_objects):
    related_objects_set = set(related_objects)

    if file.schema == "IFC2X3":
        rels = set(
            rel
            for object in related_objects_set
            if (rel := next((rel for rel in object.IsDefinedBy if rel.is_a("IfcRelDefinesByType")), None))
        )
    else:
        rels = set(rel for object in related_objects_set if (rel := next((rel for rel in object.IsTypedBy), None)))

    for rel in rels:
        related_objects_set = set(rel.RelatedObjects) - related_objects_set
        if related_objects_set:
            rel.RelatedObjects = list(related_objects_set)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
