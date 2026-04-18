# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell.util.date


def edit_work_plan(
    file: ifcopenshell.file, work_plan: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcWorkPlan

    For more information about the attributes and data types of an
    IfcWorkPlan, consult the IFC documentation.

    :param work_plan: The IfcWorkPlan entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # Let's give it a description
        ifcopenshell.api.sequence.edit_work_plan(model,
            work_plan=work_plan, attributes={"Description": "Construction of phase 1"})
    """
    for name, value in attributes.items():
        if value:
            if "Date" in name or "Time" in name:
                value = ifcopenshell.util.date.datetime2ifc(value, "IfcDateTime")
            elif name == "Duration" or name == "TotalFloat":
                value = ifcopenshell.util.date.datetime2ifc(value, "IfcDuration")
        setattr(work_plan, name, value)
