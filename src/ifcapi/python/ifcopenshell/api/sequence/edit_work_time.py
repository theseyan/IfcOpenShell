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

import ifcopenshell.util.date
from ifcopenshell.api.pset import _capi as pset_capi
from ifcopenshell.api.sequence import _capi


def edit_work_time(
    file: ifcopenshell.file,
    work_time: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
) -> None:
    """Edits the attributes of an IfcWorkTime

    For more information about the attributes and data types of an
    IfcWorkTime, consult the IFC documentation.

    :param work_time: The IfcWorkTime entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Let's create a new calendar.
        calendar = ifcopenshell.api.sequence.add_work_calendar(model)

        # Let's start defining the times that we work during the week.
        work_time = ifcopenshell.api.sequence.add_work_time(model,
            work_calendar=calendar, time_type="WorkingTimes")

        # If we don't specify any recurring time periods in our work time,
        # we need to specify a start and end date of the work time. It
        # starts at 0:00 on the start date and 24:00 at the end date.
        ifcopenshell.api.sequence.edit_work_time(model,
            work_time=work_time, attributes={"StartDate": "2000-01-01", "FinishDate": "2000-01-02"})
    """
    converted_attributes = {}
    for name, value in attributes.items():
        if name in ("Start", "StartDate"):
            value = ifcopenshell.util.date.datetime2ifc(value, "IfcDate")
        elif name in ("Finish", "FinishDate"):
            value = ifcopenshell.util.date.datetime2ifc(value, "IfcDate")
        converted_attributes[name] = value
    props = pset_capi.build_props(converted_attributes)
    try:
        _capi.call_status(
            _capi.get_lib().ifcopenshell_ifcapi_sequence_edit_work_time,
            _capi.instance_handle(work_time),
            props,
        )
    finally:
        pset_capi.free_props(props)
