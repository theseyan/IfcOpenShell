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
from ifcopenshell.api.resource import _capi


def add_resource_quantity(
    file: ifcopenshell.file,
    resource: ifcopenshell.entity_instance,
    ifc_class: str = "IfcQuantityCount",
) -> ifcopenshell.entity_instance:
    """Adds a quantity to a resource

    The quantity of a resource represents the "unit quantity" of that
    resource. For example, labour might be hired on a daily basis (8 hours).
    There are different types of quantities (e.g. volume, count, or time).
    Which quantity is used depends on the type of resource.  Material
    resources may be quantified in terms of length, area, volume, or weight.
    Equipment and labour resources are quantified in terms of time. Products
    resources are quantified in terms of counts.

    This base quantity is then used in other calculations.

    :param resource: The IfcConstructionResource to add a quantity to.
    :param ifc_class: The type of quantity to add, chosen from
        IfcQuantityArea (for material), IfcQuantityCount (for products),
        IfcQuantityLength (for material), IfcQuantityTime (for equipment or
        labour), IfcQuantityVolume (for material), and IfcQuantityWeight
        (for material).
    :return: The newly created quantity depending on the IFC class

    Example:

    .. code:: python

        # Add our own crew
        crew = ifcopenshell.api.resource.add_resource(model, ifc_class="IfcCrewResource")

        # Add some labour to our crew.
        labour = ifcopenshell.api.resource.add_resource(model,
            parent_resource=crew, ifc_class="IfcLaborResource")

        # Labour resource is quantified in terms of time.
        quantity = ifcopenshell.api.resource.add_resource_quantity(model,
            resource=labour, ifc_class="IfcQuantityTime")

        # Store the time used in hours
        ifcopenshell.api.resource.edit_resource_quantity(model,
            physical_quantity=quantity, attributes={"TimeValue": 8.0})
    """
    try:
        return _capi.call_handle(
            file,
            "resource_add_resource_quantity",
            _capi.file_handle(file),
            _capi.instance_handle(resource),
            ifc_class,
        )
    except RuntimeError as error:
        if "does not support quantity type" in str(
            error
        ) or "Invalid resource quantity class" in str(error):
            raise ValueError(str(error)) from error
        raise
