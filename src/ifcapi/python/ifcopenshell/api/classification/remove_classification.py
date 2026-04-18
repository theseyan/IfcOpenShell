# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""classification.remove_classification — remove a classification from the project."""

import ifcopenshell
import ifcopenshell.util.element


def remove_classification(file, classification):
    refs = _get_references(file, classification)
    for ref in refs:
        file.remove(ref)
    file.remove(classification)
    for rel in file.by_type("IfcRelAssociatesClassification"):
        if not rel.RelatingClassification:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    if file.schema != "IFC2X3":
        for rel in file.by_type("IfcExternalReferenceRelationship"):
            if not rel.RelatingReference:
                file.remove(rel)


def _get_references(file, classification):
    results = []
    if file.schema == "IFC2X3":
        for ref in file.by_type("IfcClassificationReference"):
            if ref.ReferencedSource == classification:
                results.append(ref)
    else:
        for ref in classification.HasReferences:
            results.append(ref)
            results.extend(_get_references(file, ref))
    return results
