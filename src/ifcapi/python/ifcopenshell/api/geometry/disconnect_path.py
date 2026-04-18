# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Optional

import ifcopenshell
import ifcopenshell.util.element


def disconnect_path(
    file: ifcopenshell.file,
    element: Optional[ifcopenshell.entity_instance] = None,
    connection_type: Optional[str] = None,
    relating_element: Optional[ifcopenshell.entity_instance] = None,
    related_element: Optional[ifcopenshell.entity_instance] = None,
) -> None:
    """There are two options to use this API method:
    - provide `element` (connected from) and `connection_type` that should be disconnected.
    - provide connected elements to disconnect explicitly:
    `relating_element` (connected from) and `related_element` (connected to)
    """
    if connection_type and element:
        connections = [
            r
            for r in element.ConnectedTo
            if r.is_a("IfcRelConnectsPathElements") and r.RelatingConnectionType == connection_type
        ] + [
            r
            for r in element.ConnectedFrom
            if r.is_a("IfcRelConnectsPathElements") and r.RelatedConnectionType == connection_type
        ]
    elif related_element:
        connections = [
            r
            for r in relating_element.ConnectedTo
            if r.is_a("IfcRelConnectsPathElements") and r.RelatedElement == related_element
        ]

    for connection in set(connections):
        history = connection.OwnerHistory
        file.remove(connection)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
