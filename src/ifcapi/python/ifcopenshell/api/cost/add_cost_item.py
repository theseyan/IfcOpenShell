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

from typing import Optional

import ifcopenshell
from ifcopenshell.api.cost import _capi


def add_cost_item(
    file: ifcopenshell.file,
    cost_schedule: Optional[ifcopenshell.entity_instance] = None,
    cost_item: Optional[ifcopenshell.entity_instance] = None,
) -> ifcopenshell.entity_instance:
    """Add a new cost item

    A cost item represents a single line item in a cost schedule. Cost items
    may then be broken down into cost subitems.

    Either `cost_schedule` or `cost_item` must be provided.

    :param cost_schedule: If the cost item is to be added as a root or top
        level cost item to a cost schedule, the IfcCostSchedule may be
        specified. This is mutually exlclusive to the cost_item parameter.
    :param cost_item: If the cost item is to be added as a subitem to an
        existing cost item, the parent IfcCostItem may be specified. This is
        mutually exclusive to the cost_schedule parameter.
    :return: The newly created IfcCostItem

    Example:

    .. code:: python

        # The very first cost item must be in a cost schedule
        schedule = ifcopenshell.api.cost.add_cost_schedule(model)

        # You may add cost items as top level item in the schedule
        item1 = ifcopenshell.api.cost.add_cost_item(model, cost_schedule=schedule)

        # Alternatively you may add them as subitems
        item2 = ifcopenshell.api.cost.add_cost_item(model, cost_item=item1)
    """
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "cost_add_cost_item",
        _capi.file_handle(file),
        _capi.instance_handle(cost_schedule),
        _capi.instance_handle(cost_item),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
