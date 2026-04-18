# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Optional

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def connect_path(
    file: ifcopenshell.file,
    relating_element: ifcopenshell.entity_instance,
    related_element: ifcopenshell.entity_instance,
    relating_connection: str = "NOTDEFINED",
    related_connection: str = "NOTDEFINED",
    description: Optional[str] = None,
    connection_geometry: Optional[ifcopenshell.entity_instance] = None,
) -> ifcopenshell.entity_instance:
    incompatible_connections: list[ifcopenshell.entity_instance] = []
    for rel in relating_element.ConnectedTo:
        if not rel.is_a("IfcRelConnectsPathElements"):
            continue
        if rel.RelatedElement == related_element:
            incompatible_connections.append(rel)
        elif rel.RelatingConnectionType in ["ATSTART", "ATEND"] and rel.RelatingConnectionType == relating_connection:
            incompatible_connections.append(rel)

    for rel in relating_element.ConnectedFrom:
        if not rel.is_a("IfcRelConnectsPathElements"):
            continue
        if rel.RelatedConnectionType in ["ATSTART", "ATEND"] and rel.RelatedConnectionType == relating_connection:
            incompatible_connections.append(rel)

    for rel in related_element.ConnectedFrom:
        if not rel.is_a("IfcRelConnectsPathElements"):
            continue
        if rel.RelatedConnectionType in ["ATSTART", "ATEND"] and rel.RelatedConnectionType == related_connection:
            incompatible_connections.append(rel)

    for rel in related_element.ConnectedTo:
        if not rel.is_a("IfcRelConnectsPathElements"):
            continue
        if rel.RelatedElement == relating_element:
            incompatible_connections.append(rel)
        elif rel.RelatingConnectionType in ["ATSTART", "ATEND"] and rel.RelatingConnectionType == related_connection:
            incompatible_connections.append(rel)

    if incompatible_connections:
        for connection in set(incompatible_connections):
            history = connection.OwnerHistory
            file.remove(connection)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    return file.create_entity(
        "IfcRelConnectsPathElements",
        ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        Description=description,
        ConnectionGeometry=connection_geometry,
        RelatingElement=relating_element,
        RelatedElement=related_element,
        RelatingConnectionType=relating_connection,
        RelatedConnectionType=related_connection,
        RelatingPriorities=[],
        RelatedPriorities=[],
    )
