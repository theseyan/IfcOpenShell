# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_resource(file, relating_resource=None, related_object=None):
    if related_object.HasAssignments:
        for assignment in related_object.HasAssignments:
            if assignment.is_a("IfcRelAssignsToResource") and assignment.RelatingResource == relating_resource:
                return assignment

    resource_of = None
    if relating_resource.ResourceOf:
        resource_of = relating_resource.ResourceOf[0]

    if resource_of:
        related_objects = list(resource_of.RelatedObjects)
        related_objects.append(related_object)
        resource_of.RelatedObjects = related_objects
        ifcopenshell.api.owner.update_owner_history(file, element=resource_of)
    else:
        resource_of = file.create_entity(
            "IfcRelAssignsToResource",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedObjects": [related_object],
                "RelatingResource": relating_resource,
            }
        )
    return resource_of
