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
from ifcopenshell.api import _relationship_capi


def update_group_products(
    file: ifcopenshell.file, group: ifcopenshell.entity_instance, products: list[ifcopenshell.entity_instance]
) -> ifcopenshell.entity_instance:
    """Sets a group products to be an explicit list of products

    Any previous products assigned to that group will have their assignment
    removed.

    :param products: A list of IfcProduct elements to assign to the group
    :param group: The IfcGroup to assign the products to
    :return: The IfcRelAssignsToGroup relationship

    Example:

    .. code:: python

        group = ifcopenshell.api.group.add_group(model, name="Furniture")
        ifcopenshell.api.group.update_group_products(model,
            products=model.by_type("IfcFurniture"), group=group)
    """
    owner_history, user, application = _relationship_capi.owner_context(file)
    product_list = _relationship_capi.instance_list(products)
    return _relationship_capi.call_handle(
        file,
        "group_update_group_products",
        _relationship_capi.file_handle(file),
        {
            "group": _relationship_capi.instance_handle(group),
            "products": product_list,
            "owner_history": _relationship_capi.instance_handle(owner_history),
            "user": _relationship_capi.instance_handle(user),
            "application": _relationship_capi.instance_handle(application),
        },
    )
