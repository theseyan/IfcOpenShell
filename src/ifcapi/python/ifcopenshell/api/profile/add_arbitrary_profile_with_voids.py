# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""profile.add_arbitrary_profile_with_voids — create an IfcArbitraryProfileDefWithVoids."""

import ifcopenshell.util.unit
from ifcopenshell.util.shape_builder import V, ifc_safe_vector_type


def add_arbitrary_profile_with_voids(file, outer_profile, inner_profiles, name=None):
    outer_profile = V(outer_profile)
    inner_profiles = [V(p) for p in inner_profiles]
    unit_scale = ifcopenshell.util.unit.calculate_unit_scale(file)
    outer_points = _convert_si_to_unit(outer_profile, unit_scale)
    inner_points = []
    for inner_profile in inner_profiles:
        inner_points.append(_convert_si_to_unit(inner_profile, unit_scale))

    inner_curves = []
    if file.schema == "IFC2X3":
        outer_curve = file.create_entity(
            "IfcPolyline",
            [file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(p)) for p in outer_points],
        )
        for inner_point in inner_points:
            inner_curves.append(
                file.create_entity(
                    "IfcPolyline",
                    [file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(p)) for p in inner_point],
                )
            )
    else:
        outer_curve = file.create_entity(
            "IfcIndexedPolyCurve",
            (file.create_entity("IfcCartesianPointList3D", ifc_safe_vector_type(outer_points))),
        )
        for inner_point in inner_points:
            dimensions = inner_point.shape[1]
            if dimensions == 2:
                ifc_points = file.create_entity("IfcCartesianPointList2D", ifc_safe_vector_type(inner_point))
            elif dimensions == 3:
                ifc_points = file.create_entity("IfcCartesianPointList3D", ifc_safe_vector_type(inner_point))
            else:
                assert False, f"Invalid dimensions: {dimensions}."
            inner_curves.append(file.create_entity("IfcIndexedPolyCurve", ifc_points))
    return file.create_entity("IfcArbitraryProfileDefWithVoids", "AREA", name, outer_curve, inner_curves)


def _convert_si_to_unit(co, unit_scale):
    return co / unit_scale
