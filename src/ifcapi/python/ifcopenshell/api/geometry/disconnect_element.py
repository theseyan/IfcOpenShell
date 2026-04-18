# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def disconnect_element(
    file: ifcopenshell.file,
    relating_element: ifcopenshell.entity_instance,
    related_element: ifcopenshell.entity_instance,
) -> None:
    incompatible_connections = []

    for rel in relating_element.ConnectedTo:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatedElement == related_element:
            incompatible_connections.append(rel)

    for rel in relating_element.ConnectedFrom:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatingElement == related_element:
            incompatible_connections.append(rel)

    for rel in related_element.ConnectedTo:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatedElement == relating_element:
            incompatible_connections.append(rel)

    for rel in related_element.ConnectedFrom:
        if rel.is_a() == "IfcRelConnectsElements" and rel.RelatingElement == relating_element:
            incompatible_connections.append(rel)

    if incompatible_connections:
        for connection in set(incompatible_connections):
            history = connection.OwnerHistory
            file.remove(connection)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
