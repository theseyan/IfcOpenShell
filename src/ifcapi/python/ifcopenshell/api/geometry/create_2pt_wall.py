# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2023 Dion Moult <dion@thinkmoult.com>
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

from typing import Any

import ifcopenshell

from . import _capi


def create_2pt_wall(
    file: ifcopenshell.file,
    element: ifcopenshell.entity_instance,
    context: ifcopenshell.entity_instance,
    p1: tuple[float, float],
    p2: tuple[float, float],
    elevation: float,
    height: float,
    thickness: float,
    is_si: bool = True,
) -> ifcopenshell.entity_instance:
    """
    Create a wall between two points (p1 and p2).
    A shortcut for geometry.add_wall_representation.

    :param element: Wall IFC element.
    :param context: IfcGeometricRepresentationContext for the representation.
        only Model/Body/MODEL_VIEW type of representations are currently supported.
    :param p1: The starting point (x, y) of the wall.
    :param p2: The ending point (x, y) of the wall.
    :param elevation: The base elevation (z-coordinate) for the wall.
    :param height: The height of the wall.
    :param thickness: The thickness of the wall.
    :param is_si: If True, provided arguments units are treated as SI (meters).
        If False, values are converted from project units to SI.
    :return: IfcShapeRepresentation.
    """
    return _capi.call_handle(
        file,
        "geometry_create_2pt_wall",
        _capi.file_handle(file),
        _capi.instance_handle(element),
        _capi.instance_handle(context),
        _capi.double_list(p1),
        _capi.double_list(p2),
        elevation,
        height,
        thickness,
        is_si,
    )


def convert_unit_to_si(co: Any, si_conversion: float) -> Any:
    return co * si_conversion
