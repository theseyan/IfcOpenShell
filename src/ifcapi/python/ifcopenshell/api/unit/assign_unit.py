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
from ifcopenshell import _ifcopenshell_capi as _capi


def assign_unit(
    file: ifcopenshell.file,
    units: Optional[list[ifcopenshell.entity_instance]] = None,
    length: Optional[dict] = None,
    area: Optional[dict] = None,
    volume: Optional[dict] = None,
) -> ifcopenshell.entity_instance:
    """Assign default project units

    Whenever a unitised quantity is specified, such as a length, area,
    voltage, pressure, etc, these project units are used by default.

    It is also possible to override units for specific properties. For
    example, generally you might want square metres for area measurements,
    but you might want square millimeters for the measurements of the cross
    sectional area of cables in cable trays. However, this function only
    deals with the default project units.

    :param units: A list of units to assign as project defaults. See
        ifcopenshell.api.unit.add_si_unit, unit.add_conversion_based_unit,
        and unit.add_monetary_unit for information on how to create units.
    :return: The IfcUnitAssignment element

    Example:

    .. code:: python

        # You need a project before you can assign units.
        ifcopenshell.api.root.create_entity(model, ifc_class="IfcProject")

        # Millimeters and square meters
        length = ifcopenshell.api.unit.add_si_unit(model, unit_type="LENGTHUNIT", prefix="MILLI")
        area = ifcopenshell.api.unit.add_si_unit(model, unit_type="AREAUNIT")

        # Optionally, add mass and time units
        mass = ifcopenshell.api.unit.add_si_unit(model, unit_type="MASSUNIT", prefix="KILO")
        time = ifcopenshell.api.unit.add_si_unit(model, unit_type="TIMEUNIT")

        # Make these the default units for the project
        ifcopenshell.api.unit.assign_unit(model, units=[length, area, mass, time])

        # Alternatively, you may specify without any arguments to
        # automatically create millimeters, square meters, and cubic meters
        # as a convenience for testing purposes. Sorry imperial folks, we
        # prioritise metric here.
        ifcopenshell.api.unit.assign_unit(model)
    """
    length = length or {"is_metric": True, "raw": "MILLIMETERS"}
    area = area or {"is_metric": True, "raw": "METERS"}
    volume = volume or {"is_metric": True, "raw": "METERS"}
    handle = _capi.unit_assign_unit(
        file._handle,
        {
            "units": None if units is None else [unit._handle for unit in units],
            "length_is_metric": length["is_metric"],
            "length_raw": length["raw"],
            "area_is_metric": area["is_metric"],
            "area_raw": area["raw"],
            "volume_is_metric": volume["is_metric"],
            "volume_raw": volume["raw"],
        },
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to assign units")
