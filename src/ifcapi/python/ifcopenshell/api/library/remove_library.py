# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_library(file, library):
    if file.schema != "IFC2X3":
        rels = []
        for reference in set(library.HasLibraryReferences):
            rels.extend(reference.LibraryRefForObjects)
            file.remove(reference)
        rels.extend(library.LibraryInfoForObjects)
        file.remove(library)
    else:
        for reference in set(library.LibraryReference or []):
            file.remove(reference)
        file.remove(library)
        rels = [rel for rel in file.by_type("IfcRelAssociatesLibrary") if rel.RelatingLibrary is None]

    for rel in rels:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
