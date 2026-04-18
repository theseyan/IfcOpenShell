# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell.util.date


def edit_work_time(
    file: ifcopenshell.file,
    work_time: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
) -> None:
    """Edits the attributes of an IfcWorkTime

    For more information about the attributes and data types of an
    IfcWorkTime, consult the IFC documentation.

    :param work_time: The IfcWorkTime entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Let's create a new calendar.
        calendar = ifcopenshell.api.sequence.add_work_calendar(model)

        # Let's start defining the times that we work during the week.
        work_time = ifcopenshell.api.sequence.add_work_time(model,
            work_calendar=calendar, time_type="WorkingTimes")

        # If we don't specify any recurring time periods in our work time,
        # we need to specify a start and end date of the work time. It
        # starts at 0:00 on the start date and 24:00 at the end date.
        ifcopenshell.api.sequence.edit_work_time(model,
            work_time=work_time, attributes={"StartDate": "2000-01-01", "FinishDate": "2000-01-02"})
    """
    for name, value in attributes.items():
        if name in ("Start", "StartDate"):
            value = ifcopenshell.util.date.datetime2ifc(value, "IfcDate")
            # 4 IfcWorktime Start
            work_time[4] = value
        elif name in ("Finish", "FinishDate"):
            value = ifcopenshell.util.date.datetime2ifc(value, "IfcDate")
            # 5 IfcWorktime Finish
            work_time[5] = value
        else:
            setattr(work_time, name, value)
