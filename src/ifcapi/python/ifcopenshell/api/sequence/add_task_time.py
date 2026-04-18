# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_task_time(
    file: ifcopenshell.file, task: ifcopenshell.entity_instance, is_recurring: bool = False
) -> ifcopenshell.entity_instance:
    """Adds a task time to a task

    Some tasks, such as activities within a work breakdown structure or
    overall maintenance tasks will have time related information. This
    includes start dates, durations, end dates, and possible recurring times
    (especially for maintenance tasks).

    :param task: The task to add time data to.
    :param is_recurring: Whether or not the time should recur.
    :return: The newly created IfcTaskTime.

    Example:

    .. code:: python

        # Let's imagine we are creating a construction schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Create a portion of a work breakdown structure.
        construction = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Construction", identification="C")
        superstructure = ifcopenshell.api.sequence.add_task(model,
            parent_task=construction, name="Superstructure", identification="C3")
        task = ifcopenshell.api.sequence.add_task(model,
            parent_task=superstructure, name="Ground Floor FRP", identification="C3.1")

        # Add time data. Note that time data is blank by default.
        time = ifcopenshell.api.sequence.add_task_time(model, task=task)

        # Let's say our task starts on the first of January when everybody
        # is still drunk from the new years celebration, and lasts for 2
        # days. Note we don't need to specify the end date, as that is
        # derived from the start plus the duration. In this simple example,
        # no calendar has been specified, so we are working 24/7. Yikes!
        ifcopenshell.api.sequence.edit_task_time(model,
            task_time=time, attributes={"ScheduleStart": "2000-01-01", "ScheduleDuration": "P2D"})
    """
    if is_recurring:
        task_time = file.create_entity("IfcTaskTimeRecurring")
    else:
        task_time = file.create_entity("IfcTaskTime")
    task.TaskTime = task_time
    return task_time
