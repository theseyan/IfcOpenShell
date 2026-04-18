# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def _ifc_safe_vector(v):
    return [float(x) for x in v]


def edit_structural_connection_cs(
    file,
    structural_item=None,
    axis=(0.0, 0.0, 1.0),
    ref_direction=(1.0, 0.0, 0.0),
):
    if structural_item.ConditionCoordinateSystem is None:
        point = file.createIfcCartesianPoint((0.0, 0.0, 0.0))
        ccs = file.createIfcAxis2Placement3D(point, None, None)
        structural_item.ConditionCoordinateSystem = ccs

    ccs = structural_item.ConditionCoordinateSystem
    if (current_axis := ccs.Axis) and file.get_total_inverses(current_axis) == 1:
        file.remove(current_axis)
    ccs.Axis = file.create_entity("IfcDirection", _ifc_safe_vector(axis))
    if (prev_ref := ccs.RefDirection) and file.get_total_inverses(prev_ref) == 1:
        file.remove(prev_ref)
    ccs.RefDirection = file.create_entity("IfcDirection", _ifc_safe_vector(ref_direction))
