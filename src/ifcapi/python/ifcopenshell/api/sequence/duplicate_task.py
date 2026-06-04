# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021-2022 Dion Moult <dion@thinkmoult.com>, Yassine Oualid <yassine@sigmadimensions.com>
#
# This self.file is part of IfcOpenShell.
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
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


# TODO: inconsistent name with other copy_xxx api methods.
def duplicate_task(
    file: ifcopenshell.file, task: ifcopenshell.entity_instance
) -> tuple[list[ifcopenshell.entity_instance], list[ifcopenshell.entity_instance]]:
    """Duplicates a task in the project

    The following relationships are also duplicated:

    * The copy will have the same attributes and property sets as the original task
    * The copy will be assigned to the parent task or work schedule
    * The copy will have duplicated nested tasks

    :param task: The task to be duplicated
    :return: A tuple that consists of two lists of tasks:

        - Original task and it's nested tasks.
        - Their corresponding duplicated tasks.

    Example:
    .. code:: python

        # We have a task
        original_task = ifcopenshell.api.sequence.add_task(
            model, work_schedule=work_schedule,
            name="Design new feature",
        )

        # And now we have two
        original_tasks, duplicated_tasks = ifcopenshell.api.sequence.duplicate_task(original_task)
        print(duplicated_tasks[0])  # A copy of ``original_task``.
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    result = _capi.sequence_duplicate_task(
        file._handle,
        task._handle,
        None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    current = ifcopenshell._take_instance_list(file, result.current)
    duplicate = ifcopenshell._take_instance_list(file, result.duplicate)
    return current, duplicate
