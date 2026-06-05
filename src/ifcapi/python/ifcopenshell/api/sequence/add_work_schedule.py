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

from datetime import datetime, time
from typing import Optional, Union

import ifcopenshell.api.owner.settings
import ifcopenshell.util.date
from ifcopenshell.api.sequence import _capi


def add_work_schedule(
    file: ifcopenshell.file,
    name: str = "Unnamed",
    predefined_type: str = "NOTDEFINED",
    object_type: Union[str, None] = None,
    start_time: Optional[Union[str, time]] = None,
    work_plan: Optional[ifcopenshell.entity_instance] = None,
) -> ifcopenshell.entity_instance:
    """Add a new work schedule

    A work schedule is a group of tasks, where the tasks are typically
    either for maintenance or for construction scheduling.

    :param name: The name of the work schedule.
    :param predefined_type: The type of schedule, chosen from ACTUAL,
        BASELINE, and PLANNED. Typically you would start with PLANNED, then
        convert to a BASELINE when changes are made with separate schedules,
        then have a parallel ACTUAL schedule.
    :param object_type: Work schedule Object Type. Should be provided
        in case if ``predefined_type`` is USERDEFINED.
    :param start_time: The earlier start time when the schedule is relevant.
        May be represented with an ISO standard string.
    :param work_plan: The IfcWorkPlan the schedule will be part of. If not
        provided, the schedule will not be grouped in a work plan and would
        exist as a top level schedule in the project. This is not
        recommended.
    :return: The newly created IfcWorkSchedule

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # Let's imagine this is one of our schedules in our work plan.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model,
            name="Construction Schedule A", work_plan=work_plan)

        # Add a root task to represent the design milestones, and major
        # project phases.
        ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Milestones", identification="A")
        ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Design", identification="B")
        construction = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Construction", identification="C")
    """
    start_time = start_time or datetime.now()
    creation_date = ifcopenshell.util.date.datetime2ifc(datetime.now(), "IfcDateTime")
    start_time = ifcopenshell.util.date.datetime2ifc(start_time, "IfcDateTime")
    user = ifcopenshell.api.owner.settings.get_user(file)
    creator_person = user.ThePerson if user else None
    owner_history, user_context, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "sequence_add_work_schedule",
        _capi.file_handle(file),
        _capi.string(name),
        _capi.string(predefined_type),
        _capi.string(object_type) if object_type is not None else None,
        _capi.string(creation_date),
        _capi.string(start_time),
        _capi.instance_handle(work_plan),
        _capi.instance_handle(creator_person),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user_context),
        _capi.instance_handle(application),
        nullable=True,
    )
