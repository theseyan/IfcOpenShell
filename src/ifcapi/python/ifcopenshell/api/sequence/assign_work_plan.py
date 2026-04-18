# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Union

import ifcopenshell
import ifcopenshell.api.aggregate
import ifcopenshell.api.project


def assign_work_plan(
    file: ifcopenshell.file, work_schedule: ifcopenshell.entity_instance, work_plan: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, None]:
    """Assigns a work schedule to a work plan

    Typically, work schedules would be assigned to a work plan at creation.
    However you may also delay this and do it manually afterwards.

    :param work_schedule: The IfcWorkSchedule that will be assigned to the
        work plan.
    :param work_plan: The IfcWorkPlan for the schedule to be assigned to.
    :return: The IfcRelAggregates relationship

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # Alternatively, if you create a schedule without a work plan ...
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # ... you can assign the work plan afterwards.
        ifcopenshell.api.sequence.assign_work_plan(work_schedule=schedule, work_plan=work_plan)
    """
    # TODO: this is an ambiguity by buildingSMART
    # See https://forums.buildingsmart.org/t/is-the-ifcworkschedule-project-declaration-mutually-exclusive-to-aggregation-within-a-relating-ifcworkplan/3510
    ifcopenshell.api.project.unassign_declaration(
        file,
        definitions=[work_schedule],
        relating_context=file.by_type("IfcContext")[0],
    )
    rel_aggregates = ifcopenshell.api.aggregate.assign_object(
        file,
        products=[work_schedule],
        relating_object=work_plan,
    )
    return rel_aggregates
