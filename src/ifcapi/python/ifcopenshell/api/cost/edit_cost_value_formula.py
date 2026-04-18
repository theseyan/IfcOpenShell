# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.cost
import ifcopenshell.util.cost


def edit_cost_value_formula(file, cost_value=None, formula=None):
    try:
        data = ifcopenshell.util.cost.unserialise_cost_value(formula or {}, cost_value)
    except Exception:
        return
    _edit_cost_value(file, data)


def _edit_cost_value(file, data, parent=None):
    ifc = data.get("ifc", None)
    if not ifc:
        ifc = ifcopenshell.api.cost.add_cost_value(file, parent=parent)
    if "AppliedValue" in data:
        if data["AppliedValue"]:
            ifc.AppliedValue = file.createIfcMonetaryMeasure(data["AppliedValue"])
        else:
            ifc.AppliedValue = None
    ifc.Category = data.get("Category", None)
    ifc.ArithmeticOperator = data.get("ArithmeticOperator", None)
    if "Components" in data:
        for component in data["Components"]:
            _edit_cost_value(file, component, ifc)
