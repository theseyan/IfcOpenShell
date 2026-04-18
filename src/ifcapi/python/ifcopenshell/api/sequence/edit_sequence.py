# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell
import ifcopenshell.api.sequence


def edit_sequence(
    file: ifcopenshell.file, rel_sequence: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcRelSequence

    For more information about the attributes and data types of an
    IfcRelSequence, consult the IFC documentation.

    :param rel_sequence: The IfcRelSequence entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Let's imagine we are creating a construction schedule. All tasks
        # need to be part of a work schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Let's imagine a root construction task
        construction = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Construction", identification="C")

        # Let's imagine we're building 2 zones, one after another.
        zone1 = ifcopenshell.api.sequence.add_task(model,
            parent_task=construction, name="Zone 1", identification="C.1")
        zone2 = ifcopenshell.api.sequence.add_task(model,
            parent_task=construction, name="Zone 2", identification="C.2")

        # Zone 1 finishes, then zone 2 starts.
        sequence = ifcopenshell.api.sequence.assign_sequence(model,
            relating_process=zone1, related_process=zone2)

        # What if they both started at the same time?
        ifcopenshell.api.sequence.edit_sequence(model,
            rel_sequence=sequence, attributes={"SequenceType": "START_START"})
    """
    for name, value in attributes.items():
        setattr(rel_sequence, name, value)
    if "SequenceType" in attributes.keys():
        ifcopenshell.api.sequence.cascade_schedule(file, task=rel_sequence.RelatedProcess)
