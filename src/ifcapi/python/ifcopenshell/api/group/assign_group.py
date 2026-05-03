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
from ifcopenshell.api import _relationship_capi


def assign_group(
    file: ifcopenshell.file, products: list[ifcopenshell.entity_instance], group: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, None]:
    """Assigns products to a group

    If a product is already assigned to the group, it will not be assigned
    twice.

    :param products: A list of IfcProduct elements to assign to the group
    :param group: The IfcGroup to assign the products to
    :return: The IfcRelAssignsToGroup relationship
        or `None` if `products` was empty list.

    Example:

    .. code:: python

        group = ifcopenshell.api.group.add_group(model, name="Furniture")
        ifcopenshell.api.group.assign_group(model,
            products=model.by_type("IfcFurniture"), group=group)
    """
    if not products:
        return

    lib = _relationship_capi.get_lib()
    owner_history, user, application = _relationship_capi.owner_context(file)
    product_list = _relationship_capi.instance_list(products)
    return _relationship_capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_group_assign_group,
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_list_ptr(product_list),
        _relationship_capi.instance_handle(group),
        _relationship_capi.instance_handle(owner_history),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
