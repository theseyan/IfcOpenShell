# SPDX-License-Identifier: LGPL-3.0-or-later

"""Minimal edit_object_placement for the ifcapi package."""

import numpy as np

import ifcopenshell
import ifcopenshell.util.unit


def edit_object_placement(file, product=None, matrix=None, is_si=True, should_transform_children=False):
    """Set the object placement of a product to the given 4x4 matrix."""
    if not hasattr(product, "ObjectPlacement"):
        return

    if matrix is None:
        matrix = np.eye(4)
    matrix = np.array(matrix, dtype=float)

    if is_si:
        unit_scale = ifcopenshell.util.unit.calculate_unit_scale(file)
        if unit_scale:
            matrix[0][3] /= unit_scale
            matrix[1][3] /= unit_scale
            matrix[2][3] /= unit_scale

    o = matrix[:3, 3].tolist()
    z = matrix[:3, 2].tolist()
    x = matrix[:3, 0].tolist()

    relative_to = None
    old_placement = product.ObjectPlacement
    if old_placement and old_placement.is_a("IfcLocalPlacement") and old_placement.PlacementRelTo:
        relative_to = old_placement.PlacementRelTo

    point = file.create_entity("IfcCartesianPoint", Coordinates=tuple(o))
    axis = file.create_entity("IfcDirection", DirectionRatios=tuple(z))
    ref_dir = file.create_entity("IfcDirection", DirectionRatios=tuple(x))
    placement_3d = file.create_entity(
        "IfcAxis2Placement3D", Location=point, Axis=axis, RefDirection=ref_dir
    )
    new_placement = file.create_entity(
        "IfcLocalPlacement", PlacementRelTo=relative_to, RelativePlacement=placement_3d
    )
    product.ObjectPlacement = new_placement

    if old_placement and file.get_total_inverses(old_placement) == 0:
        _cleanup_placement(file, old_placement)

    return new_placement


def _cleanup_placement(file, placement):
    if placement.is_a("IfcLocalPlacement") and placement.RelativePlacement:
        rp = placement.RelativePlacement
        if file.get_total_inverses(rp) == 1:
            for attr_name in ("Location", "Axis", "RefDirection"):
                child = getattr(rp, attr_name, None)
                if child and file.get_total_inverses(child) == 1:
                    file.remove(child)
            file.remove(rp)
    file.remove(placement)
