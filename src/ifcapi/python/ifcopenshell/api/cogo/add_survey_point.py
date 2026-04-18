# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.spatial
import ifcopenshell.guid


def _get_context(file, context_type, context_identifier, target_view):
    """Find a representation subcontext matching the given parameters."""
    for ctx in file.by_type("IfcGeometricRepresentationSubContext"):
        if (ctx.ContextType == context_type
                and ctx.ContextIdentifier == context_identifier
                and ctx.TargetView == target_view):
            return ctx
    return None


def add_survey_point(file, survey_point, site=None):
    context = _get_context(file, "Model", "Annotation", "MODEL_VIEW")
    shape_representation = file.createIfcShapeRepresentation(
        ContextOfItems=context,
        RepresentationIdentifier="Annotation",
        RepresentationType="Point",
        Items=[survey_point],
    )
    representation = file.createIfcProductDefinitionShape(Representations=[shape_representation])
    annotation = file.createIfcAnnotation(
        ifcopenshell.guid.new(),
        ObjectPlacement=context.WorldCoordinateSystem,
        Representation=representation,
        PredefinedType="SURVEY",
    )

    if site is None:
        site = file.by_type("IfcSite")[0]

    ifcopenshell.api.spatial.assign_container(file, relating_structure=site, products=[annotation])

    return annotation
