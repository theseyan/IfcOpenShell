# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import ifcopenshell.util.element


def remove_resource(file, resource=None):
    def remove_consider_history(root):
        history = root.OwnerHistory
        file.remove(root)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)

    for inverse in file.get_inverse(resource):
        if inverse.is_a("IfcRelNests"):
            if inverse.RelatingObject == resource:
                related_objects = inverse.RelatedObjects
                remove_consider_history(inverse)
                for related_object in related_objects:
                    ifcopenshell.api.resource.remove_resource(file, resource=related_object)
            elif inverse.RelatedObjects == (resource,):
                remove_consider_history(inverse)
        elif inverse.is_a("IfcRelAssignsToControl"):
            if len(inverse.RelatedObjects) == 1:
                remove_consider_history(inverse)
            else:
                related_objects = list(inverse.RelatedObjects)
                related_objects.remove(resource)
                inverse.RelatedObjects = related_objects
        elif inverse.is_a("IfcRelAssignsToResource"):
            if inverse.RelatingResource == resource:
                for related_object in inverse.RelatedObjects:
                    ifcopenshell.api.resource.unassign_resource(
                        file, related_object=related_object, relating_resource=resource
                    )
            elif inverse.RelatedObjects == (resource,):
                remove_consider_history(inverse)

    # Usage was added in IFC4
    if usage := getattr(resource, "Usage", None):
        file.remove(usage)
    if resource.BaseQuantity:
        ifcopenshell.api.resource.remove_resource_quantity(file, resource=resource)
    remove_consider_history(resource)
