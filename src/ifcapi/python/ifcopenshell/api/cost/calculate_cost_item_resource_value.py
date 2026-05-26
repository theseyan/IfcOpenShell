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
from ifcopenshell.api.cost import _capi


def calculate_cost_item_resource_value(file: ifcopenshell.file, cost_item: ifcopenshell.entity_instance) -> None:
    """Calculates the total cost of all resources associated with a cost item

    A cost item may have construction resources (e.g. equipment, material,
    etc) assigned to it. Construction resources may be assigned directly to
    the cost item, or assigned first to a task, and the task is then
    assigned to the cost item.

    The cost of a resource is calculated by the total sum of all of its base
    costs. If no quantity is provided, that sum is considered to be the
    total cost. Otherwise, it is considered to be a unit cost, and is then
    multiplied by the resource quantity. The quantity is either stored as a
    base quantity (such as a volume) for a things like material resources,
    or as a duration as a daily rate for labour resources.

    The final calculated cost is set as the cost item's value. Any
    previously existing values are removed.

    :param cost_item: The IfcCostItem to calculate
    :return: None

    Example:

    .. code:: python

        # First, we need a cost schedule and item
        schedule = ifcopenshell.api.cost.add_cost_schedule(model)
        item = ifcopenshell.api.cost.add_cost_item(model, cost_schedule=schedule)

        # Let's imagine we have our own formworking crew
        crew = ifcopenshell.api.resource.add_resource(model, ifc_class="IfcCrewResource")

        # ... and they need concrete
        concrete = ifcopenshell.api.resource.add_resource(model,
            ifc_class="IfcConstructionMaterialResource", parent_resource=crew)
        ifcopenshell.api.control.assign_control(model,
            relating_control=item, related_objects=[concrete])
        # ... which has a unit price of 42.0 per m3
        value = ifcopenshell.api.cost.add_cost_value(model, parent=concrete)
        ifcopenshell.api.cost.edit_cost_value(model, cost_value=value,
            attributes={"AppliedValue": 42.0})
        # ... and a volume of 200m3
        quantity = ifcopenshell.api.resource.add_resource_quantity(model,
            resource=concrete, ifc_class="IfcQuantityVolume")
        ifcopenshell.api.resource.edit_resource_quantity(model,
            physical_quantity=quantity, "attributes": {"VolumeValue": 200.0})

        # Let's say they also need some equipment
        equipment = ifcopenshell.api.resource.add_resource(model,
            ifc_class="IfcConstructionEquipmentResource", parent_resource=crew)
        ifcopenshell.api.control.assign_control(model,
            relating_control=item, related_objects=[equipment])
        # ... with a fixed price of 50,000
        value = ifcopenshell.api.cost.add_cost_value(model, parent=concrete)
        ifcopenshell.api.cost.edit_cost_value(model, cost_value=value,
            attributes={"AppliedValue": 42.0})

        # (42 * 200) + 50000 = 58400 is our calculated cost
        ifcopenshell.api.cost.calculate_cost_item_resource_value(model, cost_item=item)
    """
    lib = _capi.get_lib()
    return _capi.call_status(
        lib.ifcopenshell_ifcapi_cost_calculate_cost_item_resource_value,
        _capi.file_handle(file),
        _capi.instance_handle(cost_item),
    )
