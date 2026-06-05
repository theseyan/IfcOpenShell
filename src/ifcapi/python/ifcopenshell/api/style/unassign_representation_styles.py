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


def unassign_representation_styles(
    file: ifcopenshell.file,
    shape_representation: ifcopenshell.entity_instance,
    styles: list[ifcopenshell.entity_instance],
    should_use_presentation_style_assignment: bool = False,
) -> None:
    """Unassigns styles directly assigned to an object representation

    This does the inverse of assign_representation_styles.

    :param shape_representation: The IfcShapeRepresentation of the object
        that you want to unassign styles from.
    :param styles: A list of presentation styles, typically IfcSurfaceStyle.
        The number of items in the list should correlate with the number of
        items in the shape_representation's Items attribute. If you have
        more items than styles, the last style is used.
    :param should_use_presentation_style_assignment: This is a technical
        detail to accomodate a bug in Revit. This should always be left as
        the default of False, unless you are finding that colours aren't
        showing up in Revit. In that case, set it to True, but keep in mind
        that this is no longer a valid IFC. Blame Autodesk.
    :return: None

    Example:

    .. code:: python

        ifcopenshell.api.style.unassign_representation_styles(model,
            shape_representation=representation, styles=[style])
    """
    style_list = _capi.instance_list(styles or [])
    _capi.call_status(
        "style_unassign_representation_styles",
        "Failed to unassign representation styles",
        _capi.file_handle(file),
        _capi.instance_handle(shape_representation),
        style_list,
        should_use_presentation_style_assignment,
    )
