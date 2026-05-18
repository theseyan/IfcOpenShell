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

from typing import Any

import ifcopenshell
from ifcopenshell.api.pset import _capi as pset_capi
from ifcopenshell.api.resource import _capi


def edit_resource_time(
    file: ifcopenshell.file, resource_time: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcResourceTime

    For more information about the attributes and data types of an
    IfcResourceTime, consult the IFC documentation.

    :param resource_time: The IfcResourceTime entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Add our own crew
        crew = ifcopenshell.api.resource.add_resource(model, ifc_class="IfcCrewResource")

        # Add some labour to our crew.
        labour = ifcopenshell.api.resource.add_resource(model,
            parent_resource=crew, ifc_class="IfcLaborResource")

        # Labour resource is quantified in terms of time.
        ifcopenshell.api.resource.add_resource_quantity(model,
            resource=labour, ifc_class="IfcQuantityTime")

        # Store the unit time used in hours
        ifcopenshell.api.resource.edit_resource_quantity(model,
            physical_quantity=time, attributes={"TimeValue": 8.0})

        # Let's imagine we've used the resource for 2 days.
        time = ifcopenshell.api.resource.add_resource_time(model, resource=labour)
        ifcopenshell.api.resource.edit_resource_time(model,
            resource_time=time, attributes={"ScheduleWork": "P16H"})
    """
    if attributes.get("ScheduleWork", None) and "ScheduleFinish" in attributes.keys():
        del attributes["ScheduleFinish"]
    if attributes.get("ActualWork", None) and "ActualFinish" in attributes.keys():
        del attributes["ActualFinish"]

    props = _capi.build_resource_time_props(attributes)
    try:
        lib = _capi.get_lib()
        _capi.call_status(
            lib.ifcopenshell_ifcapi_resource_edit_resource_time,
            _capi.file_handle(file),
            _capi.instance_handle(resource_time),
            props,
        )
    finally:
        pset_capi.free_props(props)
