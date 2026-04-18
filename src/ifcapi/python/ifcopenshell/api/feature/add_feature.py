# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create a voiding, projecting, or surface feature relationship."""

import ifcopenshell
import ifcopenshell.api.aggregate
import ifcopenshell.api.geometry
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element
import ifcopenshell.util.placement


def add_feature(file, feature=None, element=None):
    """Create a relationship between a feature element and a building element.

    Handles IfcFeatureElementSubtraction (voids), IfcFeatureElementAddition
    (projections), and IfcSurfaceFeature (surface features).
    """
    if feature.is_a("IfcFeatureElementSubtraction"):
        rels = feature.VoidsElements
        ifc_class = "IfcRelVoidsElement"
    elif feature.is_a("IfcFeatureElementAddition"):
        rels = feature.ProjectsElements
        ifc_class = "IfcRelProjectsElement"
    elif feature.is_a("IfcSurfaceFeature"):
        if file.schema == "IFC4":
            return ifcopenshell.api.aggregate.assign_object(file, [feature], element)
        rels = feature.AdheresToElement
        ifc_class = "IfcRelAdheresToElement"

    if rels:
        if rels[0][4] == element:
            return rels[0]
        elif ifc_class == "IfcRelAdheresToElement" and len(rels[0].RelatedSurfaceFeatures) != 1:
            rels[0].RelatedSurfaceFeatures = list(set(rels[0].RelatedSurfaceFeatures) - {feature})
        else:
            history = rels[0].OwnerHistory
            file.remove(rels[0])
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    rel = file.create_entity(
        ifc_class,
        ifcopenshell.guid.new(),
        ifcopenshell.api.owner.create_owner_history(file),
        None,
        None,
        element,
        [feature] if ifc_class == "IfcRelAdheresToElement" else feature,
    )

    # Re-parent placement so it's relative to the host element
    if (placement := feature.ObjectPlacement) and placement.is_a("IfcLocalPlacement"):
        ifcopenshell.api.geometry.edit_object_placement(
            file,
            product=feature,
            matrix=ifcopenshell.util.placement.get_local_placement(placement),
            is_si=False,
        )

    return rel
