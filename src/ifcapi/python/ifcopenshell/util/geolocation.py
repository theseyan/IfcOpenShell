# SPDX-License-Identifier: LGPL-3.0-or-later

"""Minimal geolocation utilities for the ifcapi package."""

import math
from typing import NamedTuple, Optional

import ifcopenshell
import ifcopenshell.util.element
import ifcopenshell.util.placement


class HelmertTransformation(NamedTuple):
    e: float
    n: float
    h: float
    xaa: float
    xao: float
    scale: float
    factor_x: float
    factor_y: float
    factor_z: float


def get_helmert_transformation_parameters(ifc_file) -> Optional[HelmertTransformation]:
    if ifc_file.schema == "IFC2X3":
        project = ifc_file.by_type("IfcProject")[0]
        conversion = ifcopenshell.util.element.get_pset(project, "ePSet_MapConversion")
        if not conversion:
            return None
        e = conversion.get("Eastings", None) or 0
        n = conversion.get("Northings", None) or 0
        h = conversion.get("OrthogonalHeight", None) or 0
        xaa = conversion.get("XAxisAbscissa", None) or 0
        xao = conversion.get("XAxisOrdinate", None) or 0
        scale = conversion.get("Scale", None) or 1
        factor_x = factor_y = factor_z = 1
    else:
        conversion = ifc_file.by_type("IfcCoordinateOperation")
        if not conversion:
            return None
        conversion = conversion[0]
        if conversion.is_a("IfcMapConversion"):
            e = conversion.Eastings or 0
            n = conversion.Northings or 0
            h = conversion.OrthogonalHeight or 0
            xaa = conversion.XAxisAbscissa or 0
            xao = conversion.XAxisOrdinate or 0
            scale = conversion.Scale or 1
            if conversion.is_a() == "IfcMapConversionScaled":
                factor_x = conversion.FactorX
                factor_y = conversion.FactorY
                factor_z = conversion.FactorZ
            else:
                factor_x = factor_y = factor_z = 1
        elif conversion.is_a() == "IfcRigidOperation":
            e = conversion.FirstCoordinate.wrappedValue
            n = conversion.SecondCoordinate.wrappedValue
            h = conversion.Height or 0
            xaa = 1.0
            xao = 0.0
            scale = factor_x = factor_y = factor_z = 1
        else:
            return None

    if not xaa and not xao:
        xaa = 1.0
        xao = 0.0

    return HelmertTransformation(e, n, h, xaa, xao, scale, factor_x, factor_y, factor_z)


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
