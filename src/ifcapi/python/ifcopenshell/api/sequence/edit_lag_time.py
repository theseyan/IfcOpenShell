# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any

import ifcopenshell.api.sequence
import ifcopenshell.util.date


def edit_lag_time(file: ifcopenshell.file, lag_time: ifcopenshell.entity_instance, attributes: dict[str, Any]) -> None:
    """Edits the attributes of an IfcLagTime

    For more information about the attributes and data types of an
    IfcLagTime, consult the IFC documentation.

    :param lag_time: The IfcLagTime entity you want to edit
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

        # Let's imagine we're doing a typically formwork, reinforcement,
        # pour sequence. Let's start with the formwork. It'll take us 2
        # days.
        formwork = ifcopenshell.api.sequence.add_task(model,
            parent_task=construction, name="Formwork", identification="C.1")
        time = ifcopenshell.api.sequence.add_task_time(model, task=formwork)
        ifcopenshell.api.sequence.edit_task_time(model,
            task_time=time, attributes={"ScheduleStart": "2000-01-01", "ScheduleDuration": "P2D"})

        # Now let's do the reinforcement. It'll take us another 2 days.
        reinforcement = ifcopenshell.api.sequence.add_task(model,
            parent_task=construction, name="Reinforcement", identification="C.2")
        time = ifcopenshell.api.sequence.add_task_time(model, task=reinforcement)
        ifcopenshell.api.sequence.edit_task_time(model,
            task_time=time, attributes={"ScheduleStart": "2000-01-01", "ScheduleDuration": "P2D"})

        # Now let's say the formwork must finish before the reinforcement
        # can start. This is a typical finish to start relationship (FS).
        sequence = ifcopenshell.api.sequence.assign_sequence(model,
            relating_process=formwork, related_process=reinforcement)

        # Now typically there would be no lag time between formwork and
        # reinforcement, but let's pretend that we had to allow 1 day gap
        # for whatever reason.
        lag = ifcopenshell.api.sequence.assign_lag_time(model, rel_sequence=sequence, lag_value="P1D")

        # Or, let's make it 2 days instead.
        ifcopenshell.api.sequence.edit_lag_time(model, lag_time=lag, attributes={"LagValue": "P2D"})
    """
    for name, value in attributes.items():
        if name == "LagValue" and value is not None:
            if isinstance(value, float):
                value = file.createIfcRatioMeasure(value)
            else:
                value = file.createIfcDuration(ifcopenshell.util.date.datetime2ifc(value, "IfcDuration"))
        setattr(lag_time, name, value)
    for rel in [r for r in file.get_inverse(lag_time) if r.is_a("IfcRelSequence")]:
        ifcopenshell.api.sequence.cascade_schedule(file, task=rel.RelatedProcess)
