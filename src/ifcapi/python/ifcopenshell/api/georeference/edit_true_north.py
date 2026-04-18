# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit the true north direction on geometric contexts."""

import ifcopenshell.util.element
import ifcopenshell.util.geolocation


def edit_true_north(file, true_north=0.0):
    """Set the true north as a 2D vector or angle (degrees, anticlockwise positive)."""
    if isinstance(true_north, (float, int)):
        x, y = ifcopenshell.util.geolocation.angle2yaxis(true_north)
    elif true_north is not None:
        x, y = true_north

    for context in file.by_type("IfcGeometricRepresentationContext", include_subtypes=False):
        if context.TrueNorth and true_north is None:
            old_true_north = context.TrueNorth
            context.TrueNorth = None
            if not file.get_total_inverses(old_true_north):
                ifcopenshell.util.element.remove_deep2(file, old_true_north)
            continue

        if context.TrueNorth:
            if file.get_total_inverses(context.TrueNorth) != 1:
                context.TrueNorth = file.create_entity("IfcDirection")
        else:
            context.TrueNorth = file.create_entity("IfcDirection")
        context.TrueNorth.DirectionRatios = (x, y)
