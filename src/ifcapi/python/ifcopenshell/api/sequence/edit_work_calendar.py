# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell


def edit_work_calendar(
    file: ifcopenshell.file, work_calendar: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcWorkCalendar

    For more information about the attributes and data types of an
    IfcWorkCalendar, consult the IFC documentation.

    :param work_calendar: The IfcWorkCalendar entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Let's create a new calendar.
        calendar = ifcopenshell.api.sequence.add_work_calendar(model, name="5 Day Week")

        # Let's give it a description
        ifcopenshell.api.sequence.edit_work_calendar(model,
            work_calendar=calendar, attributes={"Description": "Monday to Friday 8 hour days"})
    """
    for name, value in attributes.items():
        setattr(work_calendar, name, value)
