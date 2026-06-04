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

from collections.abc import Sequence
from typing import Literal, Union

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi

IfcLogical = Union[bool, Literal["UNKNOWN"]]


def _logical_value(value):
    if value is True:
        return _capi.IFCOPENSHELL_LOGICAL_TRUE
    if value is False:
        return _capi.IFCOPENSHELL_LOGICAL_FALSE
    if value == "UNKNOWN":
        return _capi.IFCOPENSHELL_LOGICAL_UNKNOWN
    raise ValueError('Logical value must be True, False, or "UNKNOWN"')


def add_layer_with_style(
    file: ifcopenshell.file,
    name: str = "Unnamed",
    on: IfcLogical = "UNKNOWN",
    frozen: IfcLogical = "UNKNOWN",
    blocked: IfcLogical = "UNKNOWN",
    styles: Sequence[ifcopenshell.entity_instance] = (),
) -> ifcopenshell.entity_instance:
    """Adds a new layer with style

    :param name: The name of the layer.
    :param on: Whether layer is visible.
    :param frozen:
    :param blocked: Whether layer elements are blocked from manipulation.
    :param styles: Styles to be used as default for representation item.
    :return: The newly created IfcPresentationLayerWithStyle element

    Example:

        ifcopenshell.api.layer.add_layer_with_style(
            model,
            name="AI-WALL-FULL-DIMS-N",
            on=True,
            frozen=False,
            blocked=False,
            stlyes=[curve_style]
        )
    """
    style_list = [e._handle for e in styles]
    handle = _capi.layer_add_layer_with_style(
        file._handle,
        name,
        _logical_value(on),
        _logical_value(frozen),
        _logical_value(blocked),
        style_list,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to add styled layer")
