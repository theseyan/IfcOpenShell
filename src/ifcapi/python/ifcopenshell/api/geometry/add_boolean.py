# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
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

from __future__ import annotations

from typing import Literal

import ifcopenshell
from ifcopenshell.api.geometry import _capi


def add_boolean(
    file: ifcopenshell.file,
    first_item: ifcopenshell.entity_instance,
    second_items: list[ifcopenshell.entity_instance],
    operator: Literal["DIFFERENCE", "INTERSECTION", "UNION"] = "DIFFERENCE",
) -> list[ifcopenshell.entity_instance]:
    """Adds a boolean operation to two or more representation items

    This function protects against recursive booleans.

    After a boolean operation is made, since the items of
    IfcShapeRepresentation may be modified, it is not guaranteed that the
    RepresentationType is still valid. After performing all your booleans, it
    is recommended to run :func:`ifcopenshell.api.geometry.validate_csg` to
    ensure correctness.

    :param first_item: The IfcBooleanOperand that the operation is performed upon
    :param second_items: The IfcBooleanOperands that the operation will be
        performed with, in the order given of the list.
    :param operator: The type of boolean operation to perform
    :return: A list of newly created IfcBooleanResult in the order of boolean
        operations (based on the order of second items). If nothing was
        created, the list will be empty.
    """
    second_item_list = _capi.instance_list(second_items)
    return _capi.call_handle_list(
        file,
        "geometry_add_boolean",
        _capi.file_handle(file),
        _capi.instance_handle(first_item),
        second_item_list,
        operator,
    )
