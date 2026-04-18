# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Utility functions for classification data."""

import ifcopenshell.util.element


def get_references(element, should_inherit=True):
    results = set()
    if not element.is_a("IfcRoot"):
        references = getattr(element, "HasExternalReferences", None)
        if references is None:
            references = getattr(element, "HasExternalReference", None)
        if references is not None:
            return {r.RelatingReference for r in references}
    if should_inherit and element.is_a("IfcObject"):
        element_type = ifcopenshell.util.element.get_type(element)
        if element_type and element_type != element:
            results = get_references(element_type)
    occurrence_results = {
        r.RelatingClassification
        for r in getattr(element, "HasAssociations", []) or []
        if r.is_a("IfcRelAssociatesClassification")
    }
    if results:
        type_references_per_system = {}
        occurrence_references_per_system = {}
        for result in results:
            type_references_per_system.setdefault(get_classification(result), []).append(result)
        for result in occurrence_results:
            occurrence_references_per_system.setdefault(get_classification(result), []).append(result)
        type_references_per_system.update(occurrence_references_per_system)
        results = set()
        for values in type_references_per_system.values():
            for v in values:
                results.add(v)
        return results
    return occurrence_results


def get_classification(reference):
    if reference.is_a("IfcClassification"):
        return reference
    return get_classification(reference.ReferencedSource) if reference.ReferencedSource is not None else None
