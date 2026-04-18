# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Optional

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def connect_element(
    file: ifcopenshell.file,
    relating_element: ifcopenshell.entity_instance,
    related_element: ifcopenshell.entity_instance,
    description: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    incompatible_connections = []

    for rel in relating_element.ConnectedFrom:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatingElement == related_element:
            incompatible_connections.append(rel)

    for rel in related_element.ConnectedTo:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatedElement == relating_element:
            incompatible_connections.append(rel)

    if incompatible_connections:
        for connection in set(incompatible_connections):
            history = connection.OwnerHistory
            file.remove(connection)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    for rel in relating_element.ConnectedTo:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatedElement == related_element:
            rel.Description = description
            return rel

    return file.createIfcRelConnectsElements(
        ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        Description=description,
        RelatingElement=relating_element,
        RelatedElement=related_element,
    )
