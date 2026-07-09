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

import ifcopenshell.util.unit
from typing import Optional

from ifcopenshell.api.boundary import _capi
from ifcopenshell.util.shape_builder import SequenceOfVectors


def assign_connection_geometry(
    file: ifcopenshell.file,
    rel_space_boundary: ifcopenshell.entity_instance,
    outer_boundary: SequenceOfVectors,
    location: tuple[float, float, float],
    axis: tuple[float, float, float],
    ref_direction: tuple[float, float, float],
    inner_boundaries: Optional[SequenceOfVectors] = None,
    unit_scale: Optional[float] = None,
) -> None:
    """Create and assign a connection geometry to a space boundary relationship

    A space boundary may optionally have a plane that represents how that
    space is adjacent to another space, known as the connection geometry.
    You may specify this plane in terms of an outer boundary polyline, zero
    or more inner boundaries (such as for windows), and a positional matrix
    for the orientation of the plane.

    :param rel_space_boundary: The space boundary relationship to assign the
        connection geometry to.
    :param outer_boundary: A list of 2D points representing an open
        polyline.  The last point will connect to the first point. Each
        point is represented by an interable of 2 floats. The coordinates of
        the points are relative to the positional matrix arguments.
    :param inner_boundaries: A list of zero or more inner boundaries to use
        for the plane. Each boundary is represented by an open polyline, as
        defined by the outer_boundary argument.
    :param location: The local origin of the connection geometry, defined as
        an XYZ coordinate relative to the placement of the space that is
        being bounded.
    :param axis: The local X axis of the connection geometry, defined as an
        XYZ vector relative to the placement of the space that is being
        bounded.
    :param ref_direction: The local Z axis of the connection geometry,
        defined as an XYZ vector relative to the placement of the space that
        is being bounded. The Y vector is automatically derived using the
        right hand rule.
    :param unit_scale: The unit scale as calculated by
        ifcopenshell.util.unit.calculate_unit_scale. If not provided, it
        will be automatically calculated for you.
    :return: None

    Example:

    .. code:: python

        ifcopenshell.api.boundary.assign_connection_geometry(model,
            rel_space_boundary=element,
            outer_boundary=[(0., 0.), (1., 0.), (1., 1.), (0., 1.)],
            location=[0., 0., 0.], axis=[1., 0., 0.], ref_direction=[0., 0., 1.],
            )
    """
    _capi.call_status(
        "boundary_assign_connection_geometry",
        "Failed to assign boundary connection geometry",
        _capi.file_handle(file),
        _capi.instance_handle(rel_space_boundary),
        {
            "outer_boundary": _capi.double_list_list(outer_boundary),
            "location": _capi.double_list(location),
            "axis": _capi.double_list(axis),
            "ref_direction": _capi.double_list(ref_direction),
            "inner_boundaries": _capi.double_list_list_list(inner_boundaries or []),
            "unit_scale": unit_scale if unit_scale is not None else ifcopenshell.util.unit.calculate_unit_scale(file),
        },
    )
