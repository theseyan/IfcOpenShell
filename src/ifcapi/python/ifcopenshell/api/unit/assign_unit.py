# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.assign_unit — assign default project units."""

import ifcopenshell
import ifcopenshell.util.unit


def assign_unit(file, units=None, length=None, area=None, volume=None):
    length = length or {"is_metric": True, "raw": "MILLIMETERS"}
    area = area or {"is_metric": True, "raw": "METERS"}
    volume = volume or {"is_metric": True, "raw": "METERS"}

    if units:
        new_units = units
    else:
        new_units = [
            _create_metric_unit(file, "length", length),
            _create_metric_unit(file, "area", area),
            _create_metric_unit(file, "volume", volume),
        ]

    unit_assignment = ifcopenshell.util.unit.get_unit_assignment(file)
    if not unit_assignment:
        unit_assignment = file.createIfcUnitAssignment()
        file.by_type("IfcProject")[0].UnitsInContext = unit_assignment

    new_unit_types = [u.UnitType if not u.is_a("IfcMonetaryUnit") else u.is_a() for u in new_units]
    existing = set(
        u for u in (unit_assignment.Units or [])
        if u.is_a() not in new_unit_types and getattr(u, "UnitType", None) not in new_unit_types
    )
    for u in new_units:
        existing.add(u)
    unit_assignment.Units = list(existing)
    return unit_assignment


def _create_metric_unit(file, unit_type, data):
    type_prefix = ""
    if unit_type == "area":
        type_prefix = "SQUARE_"
    elif unit_type == "volume":
        type_prefix = "CUBIC_"
    return file.createIfcSIUnit(
        None,
        "{}UNIT".format(unit_type.upper()),
        ifcopenshell.util.unit.get_prefix(data["raw"]),
        type_prefix + ifcopenshell.util.unit.get_unit_name(data["raw"]),
    )
