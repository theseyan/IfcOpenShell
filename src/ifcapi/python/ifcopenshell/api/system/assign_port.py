# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.geometry
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.placement


def assign_port(file, element, port):
    if file.schema == "IFC2X3":
        return _execute_ifc2x3(file, element, port)

    rels = element.IsNestedBy or []

    for rel in rels:
        if port in rel.RelatedObjects:
            return rel

    if rels:
        rel = rels[0]
        related_objects = set(rel.RelatedObjects) or set()
        related_objects.add(port)
        rel.RelatedObjects = list(related_objects)
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
    else:
        rel = file.create_entity(
            "IfcRelNests",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatedObjects=[port],
            RelatingObject=element,
        )

    _update_port_placement(file, port)

    return rel


def _execute_ifc2x3(file, element, port):
    for rel in element.HasPorts or []:
        if rel.RelatingPort == port:
            return rel
    rel = file.create_entity(
        "IfcRelConnectsPortToElement",
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        RelatingPort=port,
        RelatedElement=element,
    )
    _update_port_placement(file, port)
    return rel


def _update_port_placement(file, port):
    placement = getattr(port, "ObjectPlacement", None)
    if placement and placement.is_a("IfcLocalPlacement"):
        ifcopenshell.api.geometry.edit_object_placement(
            file,
            product=port,
            matrix=ifcopenshell.util.placement.get_local_placement(port.ObjectPlacement),
            is_si=False,
        )
