# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_port(file, element, port):
    if file.schema == "IFC2X3":
        return _execute_ifc2x3(file, element, port)

    for rel in element.IsNestedBy or []:
        if port in rel.RelatedObjects:
            if len(rel.RelatedObjects) == 1:
                history = rel.OwnerHistory
                file.remove(rel)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)
                return
            related_objects = set(rel.RelatedObjects) or set()
            related_objects.remove(port)
            rel.RelatedObjects = list(related_objects)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)


def _execute_ifc2x3(file, element, port):
    for rel in element.HasPorts or []:
        if rel.RelatingPort == port:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
            return
