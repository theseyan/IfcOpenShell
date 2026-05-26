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

from typing import Union

import ifcopenshell
from ifcopenshell.api.cost import _capi


def copy_cost_item(
    file: ifcopenshell.file, cost_item: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, list[ifcopenshell.entity_instance]]:
    """Copies all cost items and related relationships

    The following relationships are also duplicated:

    * The copy will have the same attributes and property sets as the original cost item
    * The copy will be assigned to the parent cost schedule
    * The copy will have duplicated nested cost items

    :param cost_item: The cost item to be duplicated
    :return: The duplicated cost item or the list of duplicated cost items if the latter has children

    Example:
    .. code:: python

        # We have a cost item
        cost_item = CostItem(name="Design new feature", deadline="2023-03-01")

        # And now we have two
        duplicated_cost_item = project.duplicate_cost_item(cost_item)


    """
    lib = _capi.get_lib()
    copied = _capi.call_handle_list(
        file,
        lib.ifcopenshell_ifcapi_cost_copy_cost_item,
        _capi.file_handle(file),
        _capi.instance_handle(cost_item),
    )
    return copied[0] if len(copied) == 1 else copied
