# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.control
import ifcopenshell.api.project
import ifcopenshell.api.sequence
import ifcopenshell.util.element


def remove_work_calendar(file: ifcopenshell.file, work_calendar: ifcopenshell.entity_instance) -> None:
    """Removes a work calendar

    All relationships are also removed, such as if a task is set to use that
    calendar.

    :param work_calendar: The IfcWorkCalendar to remove
    :return: None

    Example:

    .. code:: python

        # Let's create a new calendar.
        calendar = ifcopenshell.api.sequence.add_work_calendar(model, name="5 Day Week")

        # And remove it immediately
        ifcopenshell.api.sequence.remove_work_calendar(model, work_calendar=calendar)
    """
    # TODO: do a deep purge
    ifcopenshell.api.project.unassign_declaration(
        file,
        definitions=[work_calendar],
        relating_context=file.by_type("IfcContext")[0],
    )
    if work_calendar.Controls:
        for rel in work_calendar.Controls:
            for related_object in rel.RelatedObjects:
                ifcopenshell.api.control.unassign_control(
                    file,
                    relating_control=work_calendar,
                    related_objects=[related_object],
                )

    # Currently in API work times are created already attached
    # to the work calendar, so they are never reused.
    for working_time in work_calendar.WorkingTimes or []:
        ifcopenshell.api.sequence.remove_work_time(file, work_time=working_time)

    for exception_time in work_calendar.ExceptionTimes or []:
        ifcopenshell.api.sequence.remove_work_time(file, work_time=exception_time)

    history = work_calendar.OwnerHistory
    file.remove(work_calendar)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
