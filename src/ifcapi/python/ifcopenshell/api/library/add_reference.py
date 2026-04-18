# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_reference(file, library):
    if file.schema == "IFC2X3":
        reference = file.createIfcLibraryReference()
        references = list(library.LibraryReference or [])
        references.append(reference)
        library.LibraryReference = references
        return reference
    return file.createIfcLibraryReference(ReferencedLibrary=library)
