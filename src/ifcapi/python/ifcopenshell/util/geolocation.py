# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Minimal geolocation utilities for the ifcapi package."""

import math

import ifcopenshell.util.placement


def angle2yaxis(angle):
    """Convert an angle (decimal degrees, anticlockwise positive) to a Y-axis unit vector."""
    angle_rad = math.radians(angle)
    x = -math.sin(angle_rad)
    y = math.cos(angle_rad)
    return x, y


def yaxis2angle(x, y):
    """Convert a Y-axis unit vector to an angle in decimal degrees."""
    return -math.degrees(math.atan2(x, y))


def get_true_north(ifc_file):
    """Return the true north angle in decimal degrees (anticlockwise positive)."""
    for context in ifc_file.by_type("IfcGeometricRepresentationContext", include_subtypes=False):
        if context.TrueNorth:
            ratios = context.TrueNorth.DirectionRatios
            return yaxis2angle(ratios[0], ratios[1])
    return 0.0


def get_wcs(ifc_file):
    """Get the WCS (prioritising 3D contexts) as a 4x4 matrix."""
    wcs = None
    for context in ifc_file.by_type("IfcGeometricRepresentationContext", include_subtypes=False):
        wcs = context.WorldCoordinateSystem
        if context.ContextType == "Model":
            break
    if wcs:
        return ifcopenshell.util.placement.get_axis2placement(wcs)
