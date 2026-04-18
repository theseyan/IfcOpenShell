# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.add_derived_unit — create an IfcDerivedUnit."""

import ifcopenshell


def add_derived_unit(file, unit_type, userdefinedtype, attributes):
    derive_unit_elements = []

    for named_unit in attributes:
        derive_unit_elements.append(
            file.create_entity("IfcDerivedUnitElement", Unit=named_unit, Exponent=attributes[named_unit])
        )

    return file.create_entity(
        "IfcDerivedUnit", Elements=derive_unit_elements, UnitType=unit_type, UserDefinedType=userdefinedtype
    )
