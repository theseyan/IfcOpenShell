# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from datetime import datetime, time
from typing import Optional, Union

import ifcopenshell.api.aggregate
import ifcopenshell.api.owner.settings
import ifcopenshell.api.project
import ifcopenshell.api.root
import ifcopenshell.api.sequence


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
    work_schedule = ifcopenshell.api.root.create_entity(
        file,
        ifc_class="IfcWorkSchedule",
        predefined_type=predefined_type,
        name=name,
    )
    work_schedule.CreationDate = ifcopenshell.api.sequence.add_date_time(file, datetime.now())
    user = ifcopenshell.api.owner.settings.get_user(file)
    if user:
        work_schedule.Creators = [user.ThePerson]
    work_schedule.StartTime = ifcopenshell.api.sequence.add_date_time(file, start_time)
    if object_type:
        work_schedule.ObjectType = object_type
    if work_plan:
        ifcopenshell.api.aggregate.assign_object(
            file,
            products=[work_schedule],
            relating_object=work_plan,
        )
    elif file.schema != "IFC2X3":
        # TODO: this is an ambiguity by buildingSMART
        # See https://forums.buildingsmart.org/t/is-the-ifcworkschedule-project-declaration-mutually-exclusive-to-aggregation-within-a-relating-ifcworkplan/3510
        context = file.by_type("IfcContext")[0]
        ifcopenshell.api.project.assign_declaration(
            file,
            definitions=[work_schedule],
            relating_context=context,
        )
    return work_schedule
