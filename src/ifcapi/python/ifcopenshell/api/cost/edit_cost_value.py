# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element
import ifcopenshell.util.unit


def edit_cost_value(file, cost_value=None, attributes=None):
    for name, value in (attributes or {}).items():
        if name == "AppliedValue" and value is not None:
            value = file.createIfcMonetaryMeasure(value)
        elif name == "UnitBasis":
            old_unit_basis = cost_value.UnitBasis
            if value:
                value_component = file.create_entity(
                    ifcopenshell.util.unit.get_unit_measure_class(value["UnitComponent"].UnitType),
                    value["ValueComponent"],
                )
                value = file.create_entity("IfcMeasureWithUnit", value_component, value["UnitComponent"])
            if old_unit_basis:
                ifcopenshell.util.element.remove_deep2(file, old_unit_basis)
        setattr(cost_value, name, value)
