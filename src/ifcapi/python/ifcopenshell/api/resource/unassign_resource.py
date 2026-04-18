# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_resource(file, relating_resource=None, related_object=None):
    for rel in related_object.HasAssignments or []:
        if not rel.is_a("IfcRelAssignsToResource") or rel.RelatingResource != relating_resource:
            continue
        if len(rel.RelatedObjects) == 1:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
            return
        related_objects = list(rel.RelatedObjects)
        related_objects.remove(related_object)
        rel.RelatedObjects = related_objects
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
