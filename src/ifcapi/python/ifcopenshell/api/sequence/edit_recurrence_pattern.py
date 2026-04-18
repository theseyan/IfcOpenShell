# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell
import ifcopenshell.util.sequence


def edit_recurrence_pattern(
    file: ifcopenshell.file, recurrence_pattern: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcRecurrencePattern

    For more information about the attributes and data types of an
    IfcRecurrencePattern, consult the IFC documentation.

    :param recurrence_pattern: The IfcRecurrencePattern entity you want to edit
    :param attributes: a dictionary of attribute names and values.
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

        # State that we work from weekdays 1 to 5 (i.e. Monday to Friday)
        ifcopenshell.api.sequence.edit_recurrence_pattern(model,
            recurrence_pattern=pattern, attributes={"WeekdayComponent": [1, 2, 3, 4, 5]})
    """
    for name, value in attributes.items():
        setattr(recurrence_pattern, name, value)

    ifcopenshell.util.sequence.is_working_day.cache_clear()
    ifcopenshell.util.sequence.is_calendar_applicable.cache_clear()
