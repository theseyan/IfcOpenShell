# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

# NOTE: This function requires numpy. If numpy is not available,
# assign_connection_geometry cannot be used.

import numpy as np


def assign_connection_geometry(file, rel_space_boundary=None, outer_boundary=None,
                               location=None, axis=None, ref_direction=None,
                               inner_boundaries=None, unit_scale=None):
    if unit_scale is None:
        unit_scale = 1.0

    outer_boundary = np.array(outer_boundary)
    location = np.array(location)
    axis = np.array(axis)
    ref_direction = np.array(ref_direction)

    outer_poly = _create_polyline(file, outer_boundary, unit_scale)
    inner_polys = tuple(
        _create_polyline(file, np.array(b), unit_scale) for b in (inner_boundaries or [])
    )
    plane = _create_plane(file, location, axis, ref_direction, unit_scale)
    curve_bounded_plane = file.createIfcCurveBoundedPlane(plane, outer_poly, inner_polys)
    connection_geometry = file.createIfcConnectionSurfaceGeometry(curve_bounded_plane)
    rel_space_boundary.ConnectionGeometry = connection_geometry


def _create_point(file, point, unit_scale):
    coords = (point / unit_scale).tolist()
    return file.create_entity("IfcCartesianPoint", coords)


def _create_polyline(file, points, unit_scale):
    if np.allclose(points[0], points[-1]):
        points = points[:-1]
    ifc_points = [_create_point(file, p, unit_scale) for p in points]
    # Close the polyline
    ifc_points.append(ifc_points[0])
    return file.createIfcPolyline(ifc_points)


def _create_plane(file, location, axis, ref_direction, unit_scale):
    return file.createIfcPlane(
        file.createIfcAxis2Placement3D(
            _create_point(file, location, unit_scale),
            file.createIfcDirection(axis.tolist()),
            file.createIfcDirection(ref_direction.tolist()),
        )
    )
