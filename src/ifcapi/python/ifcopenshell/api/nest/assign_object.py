# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.aggregate
import ifcopenshell.api.owner
import ifcopenshell.api.spatial
import ifcopenshell.guid
import ifcopenshell.util.element


def assign_object(file, related_objects, relating_object):
    if not related_objects:
        return

    ifc2x3 = file.schema == "IFC2X3"
    related_objects_set = set(related_objects)

    if ifc2x3:
        is_nested_by = next((i for i in relating_object.IsDecomposedBy if i.is_a("IfcRelNests")), None)
    else:
        is_nested_by = next((i for i in relating_object.IsNestedBy), None)

    # NOTE: maintain .RelatedObjects order as it has meaning in IFC
    previous_nests_rels = set()
    objects_without_nests = []
    objects_with_nests = []

    # check if there is anything to change
    for object in related_objects_set:
        if ifc2x3:
            object_rel = next((i for i in object.Decomposes if i.is_a("IfcRelNests")), None)
        else:
            object_rel = next(iter(object.Nests), None)

        if object_rel is None:
            objects_without_nests.append(object)
            continue

        # either is_nested_by is None or product is part of different rel
        if object_rel != is_nested_by:
            previous_nests_rels.add(object_rel)
            objects_with_nests.append(object)

        # products with already assigned nestings will be skipped

    objects_to_change = objects_without_nests + objects_with_nests
    # nothing to change
    if not objects_to_change:
        return is_nested_by

    # Can be either only nested, aggregated, or contained at the same time.
    possibly_contained = [o for o in objects_without_nests if hasattr(o, "ContainedInStructure")]
    ifcopenshell.api.spatial.unassign_container(file, products=possibly_contained)
    ifcopenshell.api.aggregate.unassign_object(file, products=objects_without_nests)

    # unassign elements from previous nests
    for nests in previous_nests_rels:
        cur_related_objects = [o for o in nests.RelatedObjects if o not in related_objects_set]
        if cur_related_objects:
            nests.RelatedObjects = list(cur_related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=nests)
        else:
            history = nests.OwnerHistory
            file.remove(nests)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    # assign elements to a new nesting
    if is_nested_by:
        cur_related_objects = list(is_nested_by.RelatedObjects)
        cur_related_objects_set = set(cur_related_objects)
        is_nested_by.RelatedObjects = cur_related_objects + [
            o for o in related_objects if o not in cur_related_objects_set
        ]
        ifcopenshell.api.owner.update_owner_history(file, element=is_nested_by)
    else:
        is_nested_by = file.create_entity(
            "IfcRelNests",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedObjects": related_objects,
                "RelatingObject": relating_object,
            }
        )

    # NOTE: Creating a nesting relationship doesn't localize the object's placement,
    # unlike assigning it to an aggregate or a container.

    return is_nested_by
