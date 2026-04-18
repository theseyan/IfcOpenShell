# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.unit


def add_footprint_representation(
    file,
    # IfcGeometricRepresentationContext
    context: ifcopenshell.entity_instance,
    # A list of IFC curves to include in the curve set
    curves: list[ifcopenshell.entity_instance],
) -> ifcopenshell.entity_instance:
    return file.createIfcShapeRepresentation(
        context,
        context.ContextIdentifier,
        "GeometricCurveSet",
        [file.createIfcGeometricCurveSet(curves)],
    )
