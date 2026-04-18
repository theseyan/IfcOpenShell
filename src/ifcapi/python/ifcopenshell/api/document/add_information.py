# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.add_information — add a document information to the project."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def add_information(file, parent=None):
    id_attribute = "DocumentId" if file.schema == "IFC2X3" else "Identification"
    information = file.create_entity("IfcDocumentInformation", **{id_attribute: "X", "Name": "Unnamed"})

    if not parent:
        projects = file.by_type("IfcProject")
        if projects:
            parent = projects[0]

    if not parent:
        raise Exception("IfcProject is not found.")

    if parent.is_a("IfcProject") or parent.is_a("IfcContext"):
        file.create_entity(
            "IfcRelAssociatesDocument",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            RelatingDocument=information,
            RelatedObjects=[parent],
        )
    elif parent.is_a("IfcDocumentInformation"):
        if parent.IsPointer:
            rel = parent.IsPointer[0]
            documents = set(rel.RelatedDocuments)
            documents.add(information)
            rel.RelatedDocuments = list(documents)
        else:
            file.create_entity(
                "IfcDocumentInformationRelationship",
                RelatingDocument=parent,
                RelatedDocuments=[information],
            )
    return information
