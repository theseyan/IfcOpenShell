# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_actor(file, relating_actor=None, related_object=None):
    if related_object.HasAssignments:
        for rel in related_object.HasAssignments:
            if rel.is_a("IfcRelAssignsToActor") and rel.RelatingActor == relating_actor:
                return rel

    rel = None

    if relating_actor.IsActingUpon:
        rel = relating_actor.IsActingUpon[0]

    if rel:
        related_objects = list(rel.RelatedObjects)
        related_objects.append(related_object)
        rel.RelatedObjects = related_objects
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
    else:
        rel = file.create_entity(
            "IfcRelAssignsToActor",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=[related_object],
            RelatingActor=relating_actor,
        )
    return rel
