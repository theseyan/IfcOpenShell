# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.add_si_unit — create a new SI unit."""

import ifcopenshell.util.unit


def add_si_unit(file, unit_type="LENGTHUNIT", prefix=None):
    name = ifcopenshell.util.unit.si_type_names.get(unit_type, None)
    return file.create_entity("IfcSIUnit", UnitType=unit_type, Name=name, Prefix=prefix)
