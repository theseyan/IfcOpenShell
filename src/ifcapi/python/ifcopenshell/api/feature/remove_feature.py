# SPDX-License-Identifier: LGPL-3.0-or-later

"""Remove a feature element and its relationship."""

import ifcopenshell.api.aggregate
import ifcopenshell.api.root
import ifcopenshell.util.element


def remove_feature(file, feature=None):
    """Remove a feature element, its void/projection rel, and any fillings."""
    if feature.is_a("IfcFeatureElementSubtraction"):
        rels = feature.VoidsElements
    elif feature.is_a("IfcFeatureElementAddition"):
        rels = feature.ProjectsElements
    elif feature.is_a("IfcSurfaceFeature"):
        if file.schema == "IFC4":
            ifcopenshell.api.aggregate.unassign_object(file, products=[feature])
            rels = []
        else:
            rels = feature.ProjectsElements

    for rel in rels:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)

    if feature.is_a("IfcOpeningElement"):
        for rel in feature.HasFillings:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    ifcopenshell.api.root.remove_product(file, product=feature)
