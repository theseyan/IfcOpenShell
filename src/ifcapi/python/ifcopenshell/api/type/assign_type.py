# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.material
import ifcopenshell.api.owner
import ifcopenshell.api.type
import ifcopenshell.guid
import ifcopenshell.util.element


def assign_type(file, related_objects, relating_type, should_map_representations=True):
    if not related_objects:
        return

    ifc2x3 = file.schema == "IFC2X3"
    related_objects_set = set(related_objects)
    if ifc2x3:
        types = next(iter(relating_type.ObjectTypeOf), None)
    else:
        types = next(iter(relating_type.Types), None)

    previous_types_rels = set()
    objects_without_types = []
    objects_with_types = []

    # check if there is anything to change
    for obj in related_objects_set:
        if ifc2x3:
            object_rel = next((i for i in obj.IsDefinedBy if i.is_a("IfcRelDefinesByType")), None)
        else:
            object_rel = next(iter(obj.IsTypedBy), None)

        if object_rel is None:
            objects_without_types.append(obj)
            continue

        # either rel doesn't exist or product is part of different rel
        if object_rel != types:
            previous_types_rels.add(object_rel)
            objects_with_types.append(obj)

    objects_to_change = objects_without_types + objects_with_types
    # nothing to change
    if not objects_to_change:
        return types

    # unassign from previous types
    for is_typed_by in previous_types_rels:
        cur_related_objects = set(is_typed_by.RelatedObjects) - related_objects_set
        if cur_related_objects:
            is_typed_by.RelatedObjects = list(cur_related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=is_typed_by)
        else:
            history = is_typed_by.OwnerHistory
            file.remove(is_typed_by)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    # assign objects to a new type
    if types:
        types.RelatedObjects = list(set(types.RelatedObjects) | related_objects_set)
        ifcopenshell.api.owner.update_owner_history(file, element=types)
    else:
        types = file.create_entity(
            "IfcRelDefinesByType",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=list(related_objects_set),
            RelatingType=relating_type,
        )

    if should_map_representations:
        if getattr(relating_type, "RepresentationMaps", None):
            for related_object in objects_to_change:
                ifcopenshell.api.type.map_type_representations(
                    file,
                    related_object=related_object,
                    relating_type=relating_type,
                )
        _map_material_usages(file, objects_to_change, relating_type)

    # Remove PredefinedType / ObjectType if existing to forbid double typing (See #7006)
    predefined_type = ifcopenshell.util.element.get_predefined_type(relating_type)
    if predefined_type != "NOTDEFINED" and predefined_type is not None:
        for obj in related_objects_set:
            obj.ObjectType = None
            if hasattr(obj, "PredefinedType"):
                obj.PredefinedType = None
    return types


def _map_material_usages(file, related_objects, relating_type):
    type_material = ifcopenshell.util.element.get_material(relating_type)
    if not type_material:
        return
    ifc_class = type_material.is_a()
    if ifc_class in ("IfcMaterialLayerSet", "IfcMaterialProfileSet"):
        ifcopenshell.api.material.assign_material(
            file,
            products=related_objects,
            type=f"{ifc_class}Usage",
        )
