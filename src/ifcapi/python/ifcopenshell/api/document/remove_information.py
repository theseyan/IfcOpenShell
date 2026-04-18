# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.remove_information — remove a document information and its references."""

import ifcopenshell
import ifcopenshell.api.document
import ifcopenshell.util.element


def remove_information(file, information):
    if file.schema == "IFC2X3":
        references = information.DocumentReferences or []
    else:
        references = information.HasDocumentReferences

    for reference in references:
        ifcopenshell.api.document.remove_reference(file, reference=reference)

    for rel in information.IsPointer or []:
        for info in rel.RelatedDocuments:
            ifcopenshell.api.document.remove_information(file, information=info)

    for rel in information.IsPointedTo or []:
        if rel.RelatedDocuments == (information,):
            file.remove(rel)

    if file.schema == "IFC2X3":
        rels = [r for r in file.by_type("IfcRelAssociatesDocument") if r.RelatingDocument == information]
    else:
        rels = information.DocumentInfoForObjects

    for rel in rels:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
    file.remove(information)
