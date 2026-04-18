# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_control(file, relating_control=None, related_objects=None):
    if not related_objects or relating_control is None:
        return None

    related_objects_set = set(related_objects)
    objects_to_assign = set()

    control_assignments = set(relating_control.Controls)
    if control_assignments:
        for obj in related_objects_set:
            existing_assignment = next((a for a in obj.HasAssignments if a in control_assignments), None)
            if existing_assignment:
                continue
            objects_to_assign.add(obj)
    else:
        objects_to_assign = related_objects_set

    if not objects_to_assign:
        return None

    controls = next(iter(relating_control.Controls), None)

    if controls:
        related_objects_new = list(controls.RelatedObjects)
        related_objects_new.extend(objects_to_assign)
        controls.RelatedObjects = related_objects_new
        ifcopenshell.api.owner.update_owner_history(file, element=controls)
    else:
        controls = file.create_entity(
            "IfcRelAssignsToControl",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=list(objects_to_assign),
            RelatingControl=relating_control,
        )
    return controls
