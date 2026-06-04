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
from ifcopenshell import _ifcopenshell_capi as _capi


def edit_cost_value_formula(file: ifcopenshell.file, cost_value: ifcopenshell.entity_instance, formula: str) -> None:
    """Sets a cost value based on a formula, similar to formulas in spreadsheets

    Costs may be made up of many components (e.g. labour, material, waste
    factor, taxes, etc). This can be easily represented in the form of a
    formula similar thta would be used in spreadsheet applications.

    For more information, see ifcopenshell.util.cost

    :param cost_value: The IfcCostValue to set the values of
    :param formula: The formula following the language of ifcopenshell.util.cost
    :return: None

    Example:

    .. code:: python

        schedule = ifcopenshell.api.cost.add_cost_schedule(model)
        item = ifcopenshell.api.cost.add_cost_item(model, cost_schedule=schedule)

        value = ifcopenshell.api.cost.add_cost_value(model, parent=item)
        ifcopenshell.api.cost.edit_cost_value_formula(model, cost_value=value,
            formula="5000 * 1.19")
    """
    _capi.cost_edit_cost_value_formula(file._handle, cost_value._handle, formula or "")
