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


import ifcopenshell
from ifcopenshell.api.style import _capi


def unassign_material_style(
    file: ifcopenshell.file,
    material: ifcopenshell.entity_instance,
    style: ifcopenshell.entity_instance,
    context: ifcopenshell.entity_instance,
) -> None:
    """Unassigns a style to a material

    This does the inverse of assign_material_style.

    :param material: The IfcMaterial which you want to unassign the style from.
    :param style: The IfcPresentationStyle (typically IfcSurfaceStyle) that
        you want to unassign from material. This will then be applied to all
        objects that have that material.
    :param context: The IfcGeometricRepresentationSubContext at which this
        style should be unassigned. Typically this is the Model BODY context.
    :return: None

    Example:

    .. code:: python

        ifcopenshell.api.style.unassign_material_style(model, material=concrete, style=style, context=body)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_style_unassign_material_style,
        "Failed to unassign material style",
        _capi.file_handle(file),
        _capi.instance_handle(material),
        _capi.instance_handle(style),
        _capi.instance_handle(context),
    )
