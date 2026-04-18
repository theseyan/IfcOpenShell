# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def create_grid_axis(file, grid=None, axis_tag="A", same_sense=True, uvw_axes="UAxes"):
    element = file.create_entity("IfcGridAxis", AxisTag=axis_tag, SameSense=same_sense)
    axes = list(getattr(grid, uvw_axes) or [])
    axes.append(element)
    setattr(grid, uvw_axes, axes)
    return element
