# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_control(file, relating_control=None, related_objects=None):
    if not related_objects or relating_control is None:
        return

    related_objects_set = set(related_objects)
    control_assignments = set(relating_control.Controls)
    rels = set(rel for obj in related_objects_set for rel in obj.HasAssignments if rel in control_assignments)

    for rel in rels:
        related_objects_new = set(rel.RelatedObjects) - related_objects_set
        if related_objects_new:
            rel.RelatedObjects = list(related_objects_new)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
