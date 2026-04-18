# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def connect_port(file, port1, port2, direction="NOTDEFINED", element=None):
    if port1 == port2:
        return

    _purge_existing_connections_to_other_ports(file, port1, port2)

    if direction == "SOURCE":
        port1.FlowDirection = "SOURCE"
        port2.FlowDirection = "SINK"
    elif direction == "SINK":
        port1.FlowDirection = "SINK"
        port2.FlowDirection = "SOURCE"
    else:
        port1.FlowDirection = direction
        port2.FlowDirection = direction

    if direction in ["SOURCE", "SOURCEANDSINK", "NOTDEFINED"]:
        _set_connected_to(file, port1, port2)
    else:
        _purge_connected_to(file, port1)

    if direction in ["SINK", "SOURCEANDSINK", "NOTDEFINED"]:
        _set_connected_from(file, port1, port2)
    else:
        _purge_connected_from(file, port1)

    _set_realising_element(port1, element)


def _purge_existing_connections_to_other_ports(file, port1, port2):
    for rel in port1.ConnectedTo or []:
        if rel.RelatedPort != port2:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    for rel in port1.ConnectedFrom or []:
        if rel.RelatingPort != port2:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    for rel in port2.ConnectedTo or []:
        if rel.RelatedPort != port1:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    for rel in port2.ConnectedFrom or []:
        if rel.RelatingPort != port1:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)


def _set_connected_to(file, port1, port2):
    if port1.ConnectedTo:
        return

    file.create_entity(
        "IfcRelConnectsPorts",
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        RelatingPort=port1,
        RelatedPort=port2,
    )


def _set_connected_from(file, port1, port2):
    if port1.ConnectedFrom:
        return

    file.create_entity(
        "IfcRelConnectsPorts",
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        RelatingPort=port2,
        RelatedPort=port1,
    )


def _purge_connected_to(file, port1):
    for rel in port1.ConnectedTo or []:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)


def _purge_connected_from(file, port1):
    for rel in port1.ConnectedFrom or []:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)


def _set_realising_element(port1, element):
    for rel in port1.ConnectedTo or []:
        rel.RealizingElement = element
    for rel in port1.ConnectedFrom or []:
        rel.RealizingElement = element
