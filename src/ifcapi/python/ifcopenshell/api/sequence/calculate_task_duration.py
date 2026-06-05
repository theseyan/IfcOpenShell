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
from ifcopenshell.api.sequence import _capi


def calculate_task_duration(file: ifcopenshell.file, task: ifcopenshell.entity_instance) -> None:
    """Calculates the task duration based on resource usage

    If a task has labour or equipment resources assigned to it, its duration
    may be parametrically derived from the scheduled work of the resource.
    For example, a labour resource with scheduled work of 10 working days
    and a resource utilisation of 200% (i.e. two labour teams) will imply
    that the task duration is 5 working days.

    If this data is not available, such as if the task has no resources,
    then nothing happens.

    :param task: The IfcTask to calculate the duration for.
    :return: None

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

        # Store the unit time used in hours
        ifcopenshell.api.resource.edit_resource_quantity(model,
            physical_quantity=quantity, attributes={"TimeValue": 8.0})

        # Let's imagine we've used the resource for 10 days with a
        # utilisation of 200%.
        time = ifcopenshell.api.resource.add_resource_time(model, resource=labour)
        ifcopenshell.api.resource.edit_resource_time(model,
            resource_time=time, attributes={"ScheduleWork": "PT80H", "ScheduleUsage": 2})

        # Let's imagine we are creating a construction schedule. All tasks
        # need to be part of a work schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Let's create a construction task. Note that the predefined type is
        # important to distinguish types of tasks.
        task = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Foundations", identification="A")

        # Assign our resource to the task.
        ifcopenshell.api.sequence.assign_process(model, relating_process=task, related_object=labour)

        # Now we can calculate the task duration based on the resource. This
        # will set task.TaskTime.ScheduleDuration to be P5D.
        ifcopenshell.api.sequence.calculate_task_duration(model, task=task)
    """
    _capi.call_status(
        "sequence_calculate_task_duration",
        _capi.file_handle(file),
        _capi.instance_handle(task),
    )
