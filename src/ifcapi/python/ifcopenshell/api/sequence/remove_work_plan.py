# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.aggregate
import ifcopenshell.api.project
import ifcopenshell.util.element


def remove_work_plan(file: ifcopenshell.file, work_plan: ifcopenshell.entity_instance) -> None:
    """Removes a work plan

    Note that schedules that are grouped under the work plan are not
    removed.

    :param work_plan: The IfcWorkPlan to remove.
    :return: None

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # And remove it immediately
        ifcopenshell.api.sequence.remove_work_plan(model, work_plan=work_plan)
    """
    ifcopenshell.api.project.unassign_declaration(
        file,
        definitions=[work_plan],
        relating_context=file.by_type("IfcContext")[0],
    )

    related_objects = [obj for rel in work_plan.IsDecomposedBy for obj in rel.RelatedObjects]
    if related_objects:
        ifcopenshell.api.aggregate.unassign_object(file, related_objects)

    history = work_plan.OwnerHistory
    file.remove(work_plan)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
