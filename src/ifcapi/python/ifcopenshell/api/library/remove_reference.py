# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_reference(file, reference):
    if file.schema != "IFC2X3":
        rels = reference.LibraryRefForObjects
    else:
        rels = [rel for rel in file.by_type("IfcRelAssociatesLibrary") if rel.RelatingLibrary == reference]

    for rel in rels:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
    file.remove(reference)
