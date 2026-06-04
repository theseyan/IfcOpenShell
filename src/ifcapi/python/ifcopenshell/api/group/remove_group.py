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
from ifcopenshell import _ifcopenshell_capi as _capi


def remove_group(file: ifcopenshell.file, group: ifcopenshell.entity_instance) -> None:
    """Removes a group

    All products assigned to the group will remain, but the relationship to
    the group will be removed.

    :param group: The IfcGroup entity you want to remove
    :return: None

    Example:

    .. code:: python

        group = ifcopenshell.api.group.add_group(model, name="Unit 1A")
        ifcopenshell.api.group.remove_group(model, group=group)
    """
    _capi.ifcopenshell_ifcapi_group_remove_group(file._handle, group._handle)
