# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.sequence


def remove_work_time(file: ifcopenshell.file, work_time: ifcopenshell.entity_instance) -> None:
    """Removes a work time

    :param work_time: The IfcWorkTime to remove.
    :return: None

    Example:

    .. code:: python

        # Let's create a new calendar.
        calendar = ifcopenshell.api.sequence.add_work_calendar(model)

        # Let's start defining the times that we work during the week.
        work_time = ifcopenshell.api.sequence.add_work_time(model,
            work_calendar=calendar, time_type="WorkingTimes")

        # And remove it immediately
        ifcopenshell.api.sequence.remove_work_time(model, work_time=work_time)
    """

    # Currently in API recurrence patterns are created during assignment
    # and removed during unassignment, so they are never reused.
    if recurrence_pattern := work_time.RecurrencePattern:
        ifcopenshell.api.sequence.unassign_recurrence_pattern(file, recurrence_pattern)

    file.remove(work_time)
