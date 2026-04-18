# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit the World Coordinate System (WCS) for all geometric contexts."""

from math import cos, radians, sin

import ifcopenshell.util.element
import ifcopenshell.util.unit


def edit_wcs(file, x=0.0, y=0.0, z=0.0, rotation=0.0, is_si=True):
    """Set the WCS translation and rotation on all geometric contexts."""
    unit_scale = ifcopenshell.util.unit.calculate_unit_scale(file)

    if abs(rotation) < 1e-9:
        xaxis_x = 1.0
        xaxis_y = 0.0
    else:
        xaxis_x = cos(radians(rotation))
        xaxis_y = sin(radians(rotation))

    if abs(x) < 1e-9 and abs(y) < 1e-9 and abs(z) < 1e-9:
        x = y = z = 0.0

    for context in file.by_type("IfcGeometricRepresentationContext", include_subtypes=False):
        old_wcs = context.WorldCoordinateSystem
        if context.CoordinateSpaceDimension == 3:
            if is_si:
                xyz = (x / unit_scale, y / unit_scale, z / unit_scale)
            else:
                xyz = (x, y, z)
            point = file.create_entity("IfcCartesianPoint", Coordinates=xyz)
            axis = file.create_entity("IfcDirection", DirectionRatios=(0.0, 0.0, 1.0))
            ref_dir = file.create_entity("IfcDirection", DirectionRatios=(xaxis_x, xaxis_y, 0.0))
            placement = file.create_entity(
                "IfcAxis2Placement3D", Location=point, Axis=axis, RefDirection=ref_dir,
            )
        elif context.CoordinateSpaceDimension == 2:
            if is_si:
                point = file.create_entity("IfcCartesianPoint", Coordinates=(x / unit_scale, y / unit_scale))
            else:
                point = file.create_entity("IfcCartesianPoint", Coordinates=(x, y))
            placement = file.create_entity(
                "IfcAxis2Placement2D", Location=point,
                RefDirection=file.create_entity("IfcDirection", DirectionRatios=(xaxis_x, xaxis_y)),
            )
        else:
            continue
        context.WorldCoordinateSystem = placement
        if file.get_total_inverses(old_wcs) == 0:
            ifcopenshell.util.element.remove_deep2(file, old_wcs)
