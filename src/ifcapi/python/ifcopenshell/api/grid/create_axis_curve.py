# SPDX-License-Identifier: LGPL-3.0-or-later

# NOTE: This function requires numpy for coordinate transformations.

import numpy as np

import ifcopenshell.util.element


def create_axis_curve(file, *, p1, p2, grid_axis, is_si=True):
    existing_curve = grid_axis.AxisCurve
    points = np.array([p1, p2], dtype=float)
    if is_si:
        unit_scale = _calculate_unit_scale(file)
        points /= unit_scale

    grid = next(i for i in file.get_inverse(grid_axis) if i.is_a("IfcGrid"))
    grid_matrix_i = np.linalg.inv(_get_local_placement(grid.ObjectPlacement))
    p1_local, p2_local = _apply_matrix(points, grid_matrix_i)

    grid_axis.AxisCurve = file.create_entity(
        "IfcPolyline",
        (
            file.create_entity("IfcCartesianPoint", p1_local[:2].tolist()),
            file.create_entity("IfcCartesianPoint", p2_local[:2].tolist()),
        ),
    )

    if existing_curve:
        ifcopenshell.util.element.remove_deep2(file, existing_curve)


def _calculate_unit_scale(file):
    """Simple unit scale calculation — returns meters-per-project-unit."""
    try:
        import ifcopenshell.util.unit
        return ifcopenshell.util.unit.get_unit_assignment(file).get("length_scale", 1.0)
    except Exception:
        return 1.0


def _get_local_placement(placement):
    """Build a 4x4 matrix from an IfcLocalPlacement chain."""
    mat = np.eye(4)
    if placement is None:
        return mat
    if placement.is_a("IfcLocalPlacement"):
        if placement.RelativePlacement:
            rp = placement.RelativePlacement
            origin = np.array(rp.Location.Coordinates)
            if len(origin) == 2:
                origin = np.array([origin[0], origin[1], 0.0])
            mat[:3, 3] = origin
            if hasattr(rp, "Axis") and rp.Axis:
                z = np.array(rp.Axis.DirectionRatios)
                if len(z) == 2:
                    z = np.array([z[0], z[1], 0.0])
                z = z / np.linalg.norm(z)
            else:
                z = np.array([0.0, 0.0, 1.0])
            if hasattr(rp, "RefDirection") and rp.RefDirection:
                x = np.array(rp.RefDirection.DirectionRatios)
                if len(x) == 2:
                    x = np.array([x[0], x[1], 0.0])
                x = x / np.linalg.norm(x)
            else:
                x = np.array([1.0, 0.0, 0.0])
            y = np.cross(z, x)
            y = y / np.linalg.norm(y)
            x = np.cross(y, z)
            mat[:3, 0] = x
            mat[:3, 1] = y
            mat[:3, 2] = z
        if placement.PlacementRelTo:
            parent = _get_local_placement(placement.PlacementRelTo)
            mat = parent @ mat
    return mat


def _apply_matrix(points, matrix):
    """Apply a 4x4 transformation matrix to an array of 3D points."""
    pts = np.array(points, dtype=float)
    if pts.shape[-1] == 2:
        pts = np.column_stack([pts, np.zeros(len(pts))])
    ones = np.ones((pts.shape[0], 1))
    pts_h = np.hstack([pts, ones])
    result = (matrix @ pts_h.T).T
    return result[:, :3]
