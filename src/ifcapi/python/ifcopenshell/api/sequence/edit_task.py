# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell


def edit_task(file: ifcopenshell.file, task: ifcopenshell.entity_instance, attributes: dict[str, Any]) -> None:
    """Edits the attributes of an IfcTask

    For more information about the attributes and data types of an
    IfcTask, consult the IFC documentation.

    :param task: The IfcTask entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Let's imagine we are creating a construction schedule. All tasks
        # need to be part of a work schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Add a root task to represent the design milestones, and major
        # project phases.
        task = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Milestones", identification="A")

        # Change the identification
        ifcopenshell.api.sequence.edit_task(model, task=task, attributes={"Identification": "M"})
    """
    for name, value in attributes.items():
        setattr(task, name, value)
