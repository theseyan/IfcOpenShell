# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import ifcopenshell
from ifcopenshell.api.grid import _capi
from ifcopenshell.util.shape_builder import VectorType


def create_axis_curve(
    file: ifcopenshell.file,
    *,
    p1: VectorType,
    p2: VectorType,
    grid_axis: ifcopenshell.entity_instance,
    is_si: bool = True,
) -> None:
    """Adds curve geometry to a grid axis to represent the axis extents

    An IFC grid will have a minimum of two axes (typically perpendicular). Each
    axis will then have a line which represents the extents of the axis.

    Points are provided as 3D coordinates in world space.
    During axis creation, the coordinates will be localized relative to IfcGrid
    and saved as 2D.

    :param p1: The first point of the grid axis
    :param p2: The second point of the grid axis
    :param grid_axis: The IfcGridAxis element to add geometry to.
    :param is_si: If true, the points are in meters, not project units

    Example:

    .. code:: python

        # A pretty standard rectangular grid, with only two axes.
        grid = ifcopenshell.api.root.create_entity(model, ifc_class="IfcGrid")
        axis_a = ifcopenshell.api.grid.create_grid_axis(model,
            axis_tag="A", uvw_axes="UAxes", grid=grid)
        axis_1 = ifcopenshell.api.grid.create_grid_axis(model,
            axis_tag="1", uvw_axes="VAxes", grid=grid)

        # By convention, alphabetic grids are horizontal, and numeric are vertical
        ifcopenshell.api.grid.create_axis_curve(
            model, p1=np.array((0., 0., 0.)), p2=np.array((10., 0., 0.)), grid_axis=axis_a)
        ifcopenshell.api.grid.create_axis_curve(
            model, p1=np.array((0., 0., 0.)), p2=np.array((0., 10., 0.)), grid_axis=axis_1)
    """
    _capi.call_status(
        "grid_create_axis_curve",
        "grid_create_axis_curve failed",
        _capi.file_handle(file),
        _capi.double_list(p1),
        _capi.double_list(p2),
        _capi.instance_handle(grid_axis),
        is_si,
    )
