# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.control
import ifcopenshell.api.sequence
import ifcopenshell.util.element


def copy_work_schedule(
    file: ifcopenshell.file,
    work_schedule: ifcopenshell.entity_instance,
) -> ifcopenshell.entity_instance:
    """Copy a work schedule.

    :param work_schedule: IfcWorkSchedule to copy.
    :return: The duplicated IfcWorkSchedule entity.

    Example:

    .. code:: python

        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")
        schedule = ifcopenshell.api.sequence.add_work_schedule(model,
            name="Construction Schedule A", work_plan=work_plan)
        new_schedule = ifcopenshell.api.sequence.copy_work_schedule(model, schedule)
    """
    # Shared code logic with copy_cost_schedule.
    new_schedule = ifcopenshell.util.element.copy(file, work_schedule)

    for rel in work_schedule.Controls:
        for task in rel.RelatedObjects:
            duplicated_tasks = ifcopenshell.api.sequence.duplicate_task(file, task)[1]
            # All other nested items are not connected to the work schedule explicitly.
            duplicated_task = duplicated_tasks[0]
            ifcopenshell.api.control.assign_control(file, new_schedule, [duplicated_task])
    return new_schedule
