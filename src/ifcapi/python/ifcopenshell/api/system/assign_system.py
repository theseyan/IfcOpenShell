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
import ifcopenshell.api.owner.settings
import ifcopenshell.util.system
from ifcopenshell import _ifcopenshell_capi as _capi


def assign_system(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    system: ifcopenshell.entity_instance,
) -> Union[ifcopenshell.entity_instance, None]:
    """Assigns distribution elements to a system

    Note that it is not necessary to assign distribution ports to a system.

    :param products: The list of IfcDistributionElements to assign to the system.
    :param system: The IfcSystem you want to assign the element to.
    :return: The IfcRelAssignsToGroup relationship
        or `None` if `products` was empty list.

    Example:

    .. code:: python

        # A completely empty distribution system
        system = ifcopenshell.api.system.add_system(model)

        # Create a duct
        duct = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcDuctSegment", predefined_type="RIGIDSEGMENT")

        # This duct is part of the system
        ifcopenshell.api.system.assign_system(model, products=[duct], system=system)
    """
    if not all(ifcopenshell.util.system.is_assignable(failed_product := product, system) for product in products):
        raise TypeError(f"You cannot assign an {failed_product.is_a()} to an {system.is_a()}")

    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    product_list = [e._handle for e in products]
    handle = _capi.ifcopenshell_ifcapi_system_assign_system(
        file._handle,
        product_list,
        system._handle,
        None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    return None
