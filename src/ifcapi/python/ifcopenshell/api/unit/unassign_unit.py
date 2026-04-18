# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.unassign_unit — unassign default project units."""

import ifcopenshell


def unassign_unit(file, units=None):
    unit_assignments = file.by_type("IfcUnitAssignment")
    if not unit_assignments:
        return
    unit_assignment = unit_assignments[0]
    units_set = set(unit_assignment.Units or [])
    units_set = units_set - set(units or [])
    if units_set:
        unit_assignment.Units = list(units_set)
        return
    file.remove(unit_assignment)
