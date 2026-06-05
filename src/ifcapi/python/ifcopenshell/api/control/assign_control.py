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

from typing import Union

import ifcopenshell
from ifcopenshell.api import _relationship_capi


def assign_control(
    file: ifcopenshell.file,
    relating_control: ifcopenshell.entity_instance,
    related_objects: list[ifcopenshell.entity_instance],
) -> Union[ifcopenshell.entity_instance, None]:
    """Assigns a planning control or constraint to a list of objects.

    IFC can describe concepts that control other objects. For example, a
    planning calendar controls the availability of working days for
    construction planning. As another example, a cost item might constrain
    or limit the ability to procure and build a product.

    This usecase lets you assign controls following the rules of the IFC
    specification. This is an advanced topic and assumes knowledge of the
    IFC concepts to determine what is allowed to control what. In the
    future, this API will likely be deprecated in favour of multiple usecase
    specific APIs.

    :param relating_control: The IfcControl entity that is creating the
        control or constraint
    :param related_objects: The list of IfcObjectDefinition that is being controlled
    :return: The newly created IfcRelAssignsToControl. If relationship already
        existed before and wasn't changed then returns None.

    Example:

    .. code:: python

        # One common usecase is to assign a calendar to a task
        calendar = ifcopenshell.api.sequence.add_work_calendar(model)
        schedule = ifcopenshell.api.sequence.add_work_schedule(model)
        task = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule)

        # All subtasks will inherit this calendar, so assigning a single
        # calendar to the root task effectively defines a "default" calendar
        ifcopenshell.api.control.assign_control(model,
            relating_control=calendar, related_objects=[task])

        # Another common example might be relating a cost item and a product
        wall = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")
        schedule = ifcopenshell.api.cost.add_cost_schedule(model)
        cost_item = ifcopenshell.api.cost.add_cost_item(model,
            cost_schedule=schedule)
        ifcopenshell.api.control.assign_control(model,
            relating_control=cost_item, related_objects=[wall])
    """
    owner_history, user, application = _relationship_capi.owner_context(file)
    object_list = _relationship_capi.instance_list(related_objects)
    return _relationship_capi.call_handle(
        file,
        "control_assign_control",
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_handle(relating_control),
        _relationship_capi.instance_list_ptr(object_list),
        _relationship_capi.instance_handle(owner_history),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
