# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def unassign_recurrence_pattern(file: ifcopenshell.file, recurrence_pattern: ifcopenshell.entity_instance) -> None:
    """Unassigns a recurrence pattern

    Note that a recurring task time must have a recurrence pattern, so if
    you remove it, be sure to clean up after this API call
    (e.g. remove IfcTaskTimeRecurring entity
    or assign a different recurrence patern to it
    or replace IfcTaskTimeRecurring with IfcTaskTime).

    :param recurrence_pattern: The IfcRecurrencePattern to remove.
    :return: None

    Example:

    .. code:: python

        # Let's create a new calendar.
        calendar = ifcopenshell.api.sequence.add_work_calendar(model)

        # Let's start defining the times that we work during the week.
        work_time = ifcopenshell.api.sequence.add_work_time(model,
            work_calendar=calendar, time_type="WorkingTimes")

        # We create a weekly recurrence
        pattern = ifcopenshell.api.sequence.assign_recurrence_pattern(model,
            parent=work_time, recurrence_type="WEEKLY")

        # Change our mind, let's just maintain it whenever we feel like it.
        ifcopenshell.api.sequence.unassign_recurrence_pattern(recurrence_pattern=pattern)
    """
    for time_period in recurrence_pattern.TimePeriods or []:
        file.remove(time_period)
    file.remove(recurrence_pattern)
