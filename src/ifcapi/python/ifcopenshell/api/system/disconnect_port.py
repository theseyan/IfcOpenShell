# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def disconnect_port(file, port):
    rels = port.ConnectedTo or ()
    rels += port.ConnectedFrom or ()

    for rel in rels:
        rel.RelatingPort.FlowDirection = None
        rel.RelatedPort.FlowDirection = None
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
