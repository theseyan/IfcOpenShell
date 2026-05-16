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
from ifcopenshell.api.style import _capi


def remove_style(file: ifcopenshell.file, style: ifcopenshell.entity_instance) -> None:
    """Removes a presentation style

    All of the presentation items of the style will also be removed.

    :param style: The IfcPresentationStyle to remove.
    :return: None

    Example:

    .. code:: python

        # Create a new surface style
        style = ifcopenshell.api.style.add_style(model)

        # Not anymore!
        ifcopenshell.api.style.remove_style(model, style=style)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_style_remove_style,
        "Failed to remove style",
        _capi.file_handle(file),
        _capi.instance_handle(style),
    )
