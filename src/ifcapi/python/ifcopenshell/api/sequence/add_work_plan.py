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

from datetime import datetime, time
from typing import Optional, Union

import ifcopenshell.api.owner.settings
import ifcopenshell.util.date
from ifcopenshell.api.sequence import _capi


def add_work_plan(
    file: ifcopenshell.file,
    name: Optional[str] = None,
    predefined_type: str = "NOTDEFINED",
    start_time: Optional[Union[str, time]] = None,
) -> ifcopenshell.entity_instance:
    """Add a new work plan

    A work plan is a group of work schedules. Since work schedules may have
    different purposes, such as for maintenance or construction scheduling,
    baseline comparison, or phasing, work plans can be used to group related
    work schedules. At a minimum, it is recommended to use work plans to
    indicate whether the work schedules are for facility management or for
    construction scheduling.

    :param name: The name of the work plan. Recommended to be "Maintenance"
        or "Construction" for the two main purposes.
    :param predefined_type: The type of work plan, used for baselining.
        Leave as "NOTDEFINED" if unsure.
    :param start_time: The earliest start time when the schedules grouped
        within the work plan are relevant.
    :return: The newly created IfcWorkPlan

    Example:

    .. code:: python

        # This will hold all our construction schedules
        work_plan = ifcopenshell.api.sequence.add_work_plan(model, name="Construction")

        # This is one of our schedules in our work plan.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model,
            name="Construction Schedule A", work_plan=work_plan)
    """
    # Preserve upstream behavior: the public start_time argument is normalised but not used.
    start_time = start_time or datetime.now()
    creation_date = ifcopenshell.util.date.datetime2ifc(datetime.now(), "IfcDateTime")
    start_time = ifcopenshell.util.date.datetime2ifc(datetime.now(), "IfcDateTime")
    user = ifcopenshell.api.owner.settings.get_user(file)
    creator_person = user.ThePerson if user else None
    owner_history, user_context, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "sequence_add_work_plan",
        _capi.file_handle(file),
        {
            "name": name,
            "predefined_type": predefined_type,
            "creation_date": creation_date,
            "start_time": start_time,
            "creator_person": _capi.instance_handle(creator_person),
            "owner_history": _capi.instance_handle(owner_history),
            "user": _capi.instance_handle(user_context),
            "application": _capi.instance_handle(application),
        },
        nullable=True,
    )
