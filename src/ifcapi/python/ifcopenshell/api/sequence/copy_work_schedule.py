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


def copy_work_schedule(
    file: ifcopenshell.file,
    work_schedule: ifcopenshell.entity_instance,
) -> ifcopenshell.entity_instance:
    """Copy a work schedule.

    :param work_schedule: IfcWorkSchedule to copy.
    :return: The duplicated IfcWorkSchedule entity.

    Example:

    .. code:: python

        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")
        schedule = ifcopenshell.api.sequence.add_work_schedule(model,
            name="Construction Schedule A", work_plan=work_plan)
        new_schedule = ifcopenshell.api.sequence.copy_work_schedule(model, schedule)
    """
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "sequence_copy_work_schedule",
        _capi.file_handle(file),
        _capi.instance_handle(work_schedule),
        {
            "owner_history": _capi.instance_handle(owner_history),
            "user": _capi.instance_handle(user),
            "application": _capi.instance_handle(application),
        },
        nullable=True,
    )
