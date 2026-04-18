# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.remove_unit — remove a unit."""

import ifcopenshell.util.element
import ifcopenshell.util.unit


def remove_unit(file, unit):
    unit_assignment = ifcopenshell.util.unit.get_unit_assignment(file)
    if unit_assignment and unit in (unit_assignment.Units or []):
        units = list(unit_assignment.Units)
        units.remove(unit)
        if units:
            unit_assignment.Units = units
        else:
            file.remove(unit_assignment)
    ifcopenshell.util.element.remove_deep2(file, unit)
