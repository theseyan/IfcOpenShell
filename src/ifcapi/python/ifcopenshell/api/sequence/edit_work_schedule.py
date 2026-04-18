# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell.util.date


def edit_work_schedule(
    file: ifcopenshell.file, work_schedule: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcWorkSchedule

    For more information about the attributes and data types of an
    IfcWorkSchedule, consult the IFC documentation.

    :param work_schedule: The IfcWorkSchedule entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # Let's imagine this is one of our schedules in our work plan.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model,
            name="Construction Schedule A", work_plan=work_plan)

        # Let's give it a description
        ifcopenshell.api.sequence.edit_work_schedule(model,
            work_schedule=work_schedule, attributes={"Description": "3 crane design option"})
    """
    for name, value in attributes.items():
        if value:
            if "Date" in name or "Time" in name:
                value = ifcopenshell.util.date.datetime2ifc(value, "IfcDateTime")
            elif name == "Duration" or name == "TotalFloat":
                value = ifcopenshell.util.date.datetime2ifc(value, "IfcDuration")
        setattr(work_schedule, name, value)
