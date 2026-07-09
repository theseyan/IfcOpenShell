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

from typing import Any

import ifcopenshell
from ifcopenshell.api.cost import _capi
from ifcopenshell.api.pset import _capi as pset_capi


def edit_cost_value(
    file: ifcopenshell.file, cost_value: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcCostValue

    For more information about the attributes and data types of an
    IfcCostValue, consult the IFC documentation.

    :param cost_value: The IfcCostValue entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        schedule = ifcopenshell.api.cost.add_cost_schedule(model)
        item = ifcopenshell.api.cost.add_cost_item(model, cost_schedule=schedule)

        # This cost item will have a total cost of 42
        value = ifcopenshell.api.cost.add_cost_value(model, parent=item)
        ifcopenshell.api.cost.edit_cost_value(model, cost_value=value,
            attributes={"AppliedValue": 42.0})
    """
    native_attributes = attributes.copy()
    has_unit_basis = "UnitBasis" in native_attributes
    unit_basis = native_attributes.pop("UnitBasis", None)

    edit_unit_basis = has_unit_basis
    clear_unit_basis = unit_basis is None
    value_component = 0.0
    unit_component = None
    if has_unit_basis and unit_basis:
        clear_unit_basis = False
        value_component = unit_basis["ValueComponent"]
        unit_component = unit_basis["UnitComponent"]

    props = pset_capi.build_props(native_attributes)
    try:
        _capi.call_status(
            "cost_edit_cost_value",
            _capi.file_handle(file),
            _capi.instance_handle(cost_value),
            props,
            {
                "edit_unit_basis": edit_unit_basis,
                "clear_unit_basis": clear_unit_basis,
                "value_component": float(value_component),
                "unit_component": _capi.instance_handle(unit_component),
            },
        )
    finally:
        pset_capi.free_props(props)
