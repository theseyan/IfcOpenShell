# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.add_reference — create a reference to a document."""

import ifcopenshell


def add_reference(file, information):
    if file.schema == "IFC2X3":
        reference = file.create_entity("IfcDocumentReference", ItemReference="X")
        if information:
            references = list(information.DocumentReferences or [])
            references.append(reference)
            information.DocumentReferences = references
        return reference
    return file.create_entity("IfcDocumentReference", ReferencedDocument=information, Identification="X")
