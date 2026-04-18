# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.remove_reference — remove a document reference."""

import ifcopenshell
import ifcopenshell.util.element


def remove_reference(file, reference):
    if file.schema == "IFC2X3":
        rels = [r for r in file.get_inverse(reference) if r.is_a("IfcRelAssociatesDocument")]
    else:
        rels = reference.DocumentRefForObjects

    for rel in rels:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
    file.remove(reference)
