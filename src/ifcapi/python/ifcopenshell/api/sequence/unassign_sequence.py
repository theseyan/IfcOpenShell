# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.sequence
import ifcopenshell.util.element


def unassign_sequence(
    file: ifcopenshell.file,
    relating_process: ifcopenshell.entity_instance,
    related_process: ifcopenshell.entity_instance,
) -> None:
    """Removes a sequence relationship between tasks

    :param relating_process: The previous / predecessor task.
    :param related_process: The next / successor task.
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
        ifcopenshell.api.sequence.assign_sequence(model, relating_process=zone1, related_process=zone2)

        # Let's make them unrelated
        ifcopenshell.api.sequence.unassign_sequence(model,
            relating_process=zone1, related_process=zone2)
    """
    for rel in related_process.IsSuccessorFrom or []:
        if rel.RelatingProcess == relating_process:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    ifcopenshell.api.sequence.cascade_schedule(file, task=related_process)
