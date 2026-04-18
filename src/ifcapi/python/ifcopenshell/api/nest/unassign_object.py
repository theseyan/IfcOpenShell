# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_object(file, related_objects):
    # NOTE: maintain .RelatedObjects order as it has meaning in IFC
    related_objects_set = set(related_objects)
    ifc2x3 = file.schema == "IFC2X3"
    if ifc2x3:
        rels = set(
            rel
            for object in related_objects_set
            if (rel := next((rel for rel in object.Decomposes if rel.is_a("IfcRelNests")), None))
        )
    else:
        rels = set(rel for object in related_objects if (rel := next((rel for rel in object.Nests), None)))

    for rel in rels:
        cur_related_objects = [o for o in rel.RelatedObjects if o not in related_objects_set]
        if cur_related_objects:
            rel.RelatedObjects = cur_related_objects
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
