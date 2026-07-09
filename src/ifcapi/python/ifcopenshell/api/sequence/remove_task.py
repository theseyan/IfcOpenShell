# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import ifcopenshell
from ifcopenshell.api.sequence import _capi


def remove_task(file: ifcopenshell.file, task: ifcopenshell.entity_instance) -> None:
    """Removes a task

    All subtasks are also removed recursively. Any relationships such as
    sequences or controls are also removed.

    :param task: The IfcTask to remove.
    :return: None

    Example:

    .. code:: python

        # Let's imagine we are creating a construction schedule. All tasks
        # need to be part of a work schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Add a root task to represent the design milestones, and major
        # project phases.
        ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Milestones", identification="A")
        design = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Design", identification="B")
        ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Construction", identification="C")

        # Ah, let's delete the design section, who needs it anyway we'll
        # just fix it on site.
        ifcopenshell.api.sequence.remove_task(model, task=design)
    """
    _, user, application = _capi.owner_context(file)
    _capi.call_status(
        "sequence_remove_task",
        _capi.file_handle(file),
        _capi.instance_handle(task),
        {
            "user": _capi.instance_handle(user),
            "application": _capi.instance_handle(application),
        },
    )
