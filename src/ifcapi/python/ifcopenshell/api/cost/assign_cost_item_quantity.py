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


def assign_cost_item_quantity(
    file: ifcopenshell.file,
    cost_item: ifcopenshell.entity_instance,
    products: list[ifcopenshell.entity_instance],
    prop_name: str = "",
) -> None:
    """Adds a cost item quantity that is parametrically connected to a product

    A cost item may have its subtotal calculated by multiplying a unit value
    by a quantity associated with the cost item. That quantity may be either
    manually specified or parametrically connected to a quantity on a
    product. This API function lets you create that parametric connection.

    For example, you may wish to have a cost item linked to the "NetVolume"
    quantity on all IfcSlabs. Each quantity has a name which you can
    specify. If the quantity is updated in-place (which should occur for
    Native IFC applications) then the quantity for the cost item will
    automatically update as well. If the quantity is deleted and then
    re-added, then the parametric relationship is also lost.

    This API also automatically assigns a control relationship between the
    cost item and the product, so it is not necessary to use
    ifcopenshell.api.control.assign_control.

    If cost item has just 1 quantity and it's IfcQuantityCount, API will
    assume that quantity is used for counting controlled objects
    and it will recalculate the quantity value at the end of the API call
    as long as the RelatedObjects are not IfcConstructionResource which do not
    count towards the cost item (they only provide value).

    :param cost_item: The IfcCostItem to assign parametric quantities to
    :param products: The IfcObjects to assign parametric quantities to
    :param prop_name: The name of the quantity. If this is not specified,
        then it is assumed that there is no calculated quantity, and the
        number of objects are counted instead.
    :return: None

    Example:

    .. code:: python

        schedule = ifcopenshell.api.cost.add_cost_schedule(model)
        item = ifcopenshell.api.cost.add_cost_item(model, cost_schedule=schedule)

        # Let's imagine a unit cost of 5.0 per unit volume
        value = ifcopenshell.api.cost.add_cost_value(model, parent=item)
        ifcopenshell.api.cost.edit_cost_value(model, cost_value=value,
            attributes={"AppliedValue": 5.0})

        slab = ifcopenshell.api.root.create_entity(model, ifc_class="IfcSlab")
        # Usually the quantity would be automatically calculated via a
        # graphical authoring application but let's assign a manual quantity
        # for now.
        qto = ifcopenshell.api.pset.add_qto(model, product=slab, name="Qto_SlabBaseQuantities")
        ifcopenshell.api.pset.edit_qto(model, qto=qto, properties={"NetVolume": 42.0})

        # Now let's parametrically link the slab's quantity to the cost
        # item. If the slab is edited in the future and 42.0 changes, then
        # the updated value will also automatically be applied to the cost
        # item.
        ifcopenshell.api.cost.assign_cost_item_quantity(model,
            cost_item=item, products=[slab], prop_name="NetVolume")
    """
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_status(
        "cost_assign_cost_item_quantity",
        _capi.file_handle(file),
        _capi.instance_handle(cost_item),
        _capi.instance_list(products or []),
        _capi.string(prop_name),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
