# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def _ifc_safe_vector(v):
    return [float(x) for x in v]


def edit_structural_item_axis(file, structural_item=None, axis=(0.0, 0.0, 1.0)):
    axis_dir = structural_item.Axis
    if axis_dir and file.get_total_inverses(axis_dir) == 1:
        file.remove(axis_dir)
    structural_item.Axis = file.create_entity("IfcDirection", _ifc_safe_vector(axis))
