# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021-2022 Dion Moult <dion@thinkmoult.com>, Yassine Oualid <yassine@sigmadimensions.com>
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

from typing import Optional

import ifcopenshell
from ifcopenshell.api.sequence import _capi


def create_baseline(
    file: ifcopenshell.file, work_schedule: ifcopenshell.entity_instance, name: Optional[str] = None
) -> None:
    """Creates a baseline for your Work Schedule

    Using a IfcWorkSchdule having PredefinedType=PLANNED,
    We can create a baseline for our work schedule. This IfcWorkSchedule will have PredefinedType=BASELINE
    and the IfcWorkSchedule.CreationDate indicating the date of the baseline creation, and IfcWorkSchedule.Name indicating the name of the baseline.

    The following relationships are also baselined:

    * Same Tasks & attributes
    * Same Task Relationships
    * Same Construction Resources
    * Same Resource Relationships

    :param work_schedule: The planned work_schedule to baseline
    :param name: baseline work schedule name
    :return: The baseline work_schedule

    Example:

    .. code:: python

        # We have a Work Schedule
        planned_work_schedule = WorkSchedule(name="Design new feature",predefinedType="PLANNED", deadline="2023-03-01")

        # And now we have a baseline for our Work Schedule
        baseline_work_schedule = ifcopenshell.api.sequence.create_baseline(file, work_schedule=planned_work_schedule, name="Baseline 1")
    """
    owner_history, user, application = _capi.owner_context(file)
    _capi.call_status(
        _capi.get_lib().ifcopenshell_ifcapi_sequence_create_baseline,
        _capi.file_handle(file),
        _capi.instance_handle(work_schedule),
        _capi.string(name) if name is not None else None,
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
