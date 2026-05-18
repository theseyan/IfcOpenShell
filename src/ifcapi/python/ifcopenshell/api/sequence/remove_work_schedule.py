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


def remove_work_schedule(file: ifcopenshell.file, work_schedule: ifcopenshell.entity_instance) -> None:
    """Removes a work schedule

    All tasks in the work schedule are also removed recursively.

    :param work_schedule: The IfcWorkSchedule to remove.
    :return: None

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # Let's imagine this is one of our schedules in our work plan.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model,
            name="Construction Schedule A", work_plan=work_plan)

        # And remove it immediately
        ifcopenshell.api.sequence.remove_work_schedule(model, work_schedule=schedule)
    """
    _, user, application = _capi.owner_context(file)
    _capi.call_status(
        _capi.get_lib().ifcopenshell_ifcapi_sequence_remove_work_schedule,
        _capi.file_handle(file),
        _capi.instance_handle(work_schedule),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
