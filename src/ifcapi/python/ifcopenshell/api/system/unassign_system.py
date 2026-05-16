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
import ctypes
from ifcopenshell.api.system import _capi


def unassign_system(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    system: ifcopenshell.entity_instance,
) -> None:
    """Unassigns list of products from a system

    :param products: The list of IfcDistributionElements to unassign from the system.
    :param system: The IfcSystem you want to unassign the element from.
    :return: None

    Example:

    .. code:: python

        # A completely empty distribution system
        system = ifcopenshell.api.system.add_system(model)

        # Create a duct
        duct = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcDuctSegment", predefined_type="RIGIDSEGMENT")

        # This duct is part of the system
        ifcopenshell.api.system.assign_system(model, products=[duct], system=system)

        # Not anymore!
        ifcopenshell.api.system.unassign_system(model, products=[duct], system=system)
    """
    lib = _capi.get_lib()
    _, user, application = _capi.owner_context(file)
    product_list = _capi.instance_list(products)
    _capi.call_status(
        lib.ifcopenshell_ifcapi_system_unassign_system,
        "Failed to unassign system",
        _capi.file_handle(file),
        ctypes.byref(product_list),
        _capi.instance_handle(system),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
