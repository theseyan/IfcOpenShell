# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""profile.add_arbitrary_profile — create an IfcArbitraryClosedProfileDef."""

import ifcopenshell.util.unit
from ifcopenshell.util.shape_builder import V, ifc_safe_vector_type


def add_arbitrary_profile(file, profile, name=None):
    profile = V(profile)
    unit_scale = ifcopenshell.util.unit.calculate_unit_scale(file)
    points = _convert_si_to_unit(profile, unit_scale)
    if file.schema == "IFC2X3":
        curve = file.create_entity(
            "IfcPolyline",
            [file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(p)) for p in points],
        )
    else:
        dimensions = points.shape[1]
        if dimensions == 2:
            ifc_points = file.create_entity("IfcCartesianPointList2D", ifc_safe_vector_type(points))
        elif dimensions == 3:
            ifc_points = file.create_entity("IfcCartesianPointList3D", ifc_safe_vector_type(points))
        else:
            assert False, f"Invalid dimensions: {dimensions}."
        curve = file.create_entity("IfcIndexedPolyCurve", ifc_points)
    return file.create_entity("IfcArbitraryClosedProfileDef", "AREA", name, curve)


def _convert_si_to_unit(co, unit_scale):
    return co / unit_scale
