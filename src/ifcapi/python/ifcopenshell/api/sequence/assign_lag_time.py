# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.date


def assign_lag_time(
    file: ifcopenshell.file, rel_sequence: ifcopenshell.entity_instance, lag_value: str, duration_type: str = "WORKTIME"
) -> ifcopenshell.entity_instance:
    """Assign a lag time to a sequence relationship between tasks

    A task sequence (e.g. finish to start) may optionally have a lag time
    defined. This is a fundamental concept in construction scheduling. The
    lag is defined as a duration, and the duration is typically either
    calendar based (i.e.  follows the working times and holidays of the
    calendar) or elapsed time based (i.e. 24/7).

    A sequence may only have a single lag time defined. Negative lag times
    are allowed.

    :param rel_sequence: The IfcRelSequence to assign the lag time to.
    :param lag_value: An ISO standardised duration string.
    :param duration_type: Choose from WORKTIME for the associated
        calendar-based lag times (this is the most common scenario and is
        recommended as a default), or ELAPSEDTIME to not follow the
        calendar. You may also choose NOTDEFINED but the behaviour of this
        is unclear.
    :return: The newly created IfcLagTime

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
        ifcopenshell.api.sequence.assign_lag_time(model, rel_sequence=sequence, lag_value="P1D")
    """
    duration = file.create_entity("IfcDuration", ifcopenshell.util.date.datetime2ifc(lag_value, "IfcDuration"))
    lag_time = file.create_entity("IfcLagTime", DurationType=duration_type, LagValue=duration)
    if rel_sequence.is_a("IfcRelSequence"):
        if (current_lag_time := rel_sequence.TimeLag) and file.get_total_inverses(current_lag_time) == 1:
            file.remove(current_lag_time)
    rel_sequence.TimeLag = lag_time
    return lag_time
