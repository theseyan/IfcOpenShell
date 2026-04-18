# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.add_context_dependent_unit — create an IfcContextDependentUnit."""

import ifcopenshell


def add_context_dependent_unit(file, unit_type="USERDEFINED", name="THINGAMAJIG", dimensions=(0, 0, 0, 0, 0, 0, 0)):
    return file.create_entity(
        "IfcContextDependentUnit",
        Dimensions=file.createIfcDimensionalExponents(*dimensions),
        UnitType=unit_type,
        Name=name,
    )
