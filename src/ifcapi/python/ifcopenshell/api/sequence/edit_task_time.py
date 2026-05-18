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

import ifcopenshell.api.resource
import ifcopenshell.util.constraint
import ifcopenshell.util.date
import ifcopenshell.util.sequence
from ifcopenshell.api.pset import _capi as pset_capi
from ifcopenshell.api.sequence import _capi


def edit_task_time(
    file: ifcopenshell.file,
    task_time: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
) -> None:
    """Edits the attributes of an IfcTaskTime

    For more information about the attributes and data types of an
    IfcTaskTime, consult the IFC documentation.

    :param task_time: The IfcTaskTime entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Let's imagine we are creating a construction schedule. All tasks
        # need to be part of a work schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Create a task to do formwork
        task = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Formwork", identification="A")

        # Let's say it takes 2 days and starts on the 1st of January, 2000
        time = ifcopenshell.api.sequence.add_task_time(model, task=formwork)
        ifcopenshell.api.sequence.edit_task_time(model,
            task_time=time, attributes={"ScheduleStart": "2000-01-01", "ScheduleDuration": "P2D"})
    """
    if attributes.get("ScheduleDuration", None) and "ScheduleFinish" in attributes.keys():
        del attributes["ScheduleFinish"]

    converted_attributes = {}
    for name, value in attributes.items():
        if value is not None:
            if "Start" in name or "Finish" in name or name == "StatusTime":
                value = ifcopenshell.util.date.datetime2ifc(value, "IfcDateTime")
            elif name == "ScheduleDuration" or name == "ActualDuration" or name == "RemainingTime":
                value = ifcopenshell.util.date.datetime2ifc(value, "IfcDuration")
        converted_attributes[name] = value

    props = pset_capi.build_props(converted_attributes)
    try:
        _capi.call_status(
            _capi.get_lib().ifcopenshell_ifcapi_sequence_edit_task_time,
            _capi.file_handle(file),
            _capi.instance_handle(task_time),
            props,
        )
    finally:
        pset_capi.free_props(props)

    if task_time.ScheduleDuration:
        task = next(e for e in file.get_inverse(task_time) if e.is_a("IfcTask"))
        resources = ifcopenshell.util.sequence.get_task_resources(task, is_recursive=False)
        for resource in resources:
            if ifcopenshell.util.constraint.is_attribute_locked(resource, "Usage.ScheduleWork"):
                ifcopenshell.api.resource.calculate_resource_usage(file, resource=resource)
            # TODO: If the duration changes, this implies the productivity rate must change to accomModate the new Schedule Work to be calculated.
            # elif ifcopenshell.util.constraint.is_attribute_locked(resource, "Usage.ScheduleUsage"):
            #     ifcopenshell.api.resource.calculate_resource_work(file, resource=resource)
