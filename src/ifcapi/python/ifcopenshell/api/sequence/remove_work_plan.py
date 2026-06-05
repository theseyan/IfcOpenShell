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
    _, user, application = _capi.owner_context(file)
    _capi.call_status(
        "sequence_remove_work_plan",
        _capi.file_handle(file),
        _capi.instance_handle(work_plan),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
