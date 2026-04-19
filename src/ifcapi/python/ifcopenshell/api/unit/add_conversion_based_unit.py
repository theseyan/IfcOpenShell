# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.add_conversion_based_unit — create a conversion-based unit."""

import ifcopenshell
import ifcopenshell.util.unit

imperial_types = {
    "inch": "LENGTHUNIT", "foot": "LENGTHUNIT", "yard": "LENGTHUNIT", "mile": "LENGTHUNIT",
    "thou": "LENGTHUNIT",
    "square inch": "AREAUNIT", "square foot": "AREAUNIT", "square yard": "AREAUNIT",
    "square mile": "AREAUNIT", "acre": "AREAUNIT",
    "cubic inch": "VOLUMEUNIT", "cubic foot": "VOLUMEUNIT", "cubic yard": "VOLUMEUNIT",
    "cubic mile": "VOLUMEUNIT", "litre": "VOLUMEUNIT",
}

named_dimensions = {
    "LENGTHUNIT": (1, 0, 0, 0, 0, 0, 0),
    "AREAUNIT": (2, 0, 0, 0, 0, 0, 0),
    "VOLUMEUNIT": (3, 0, 0, 0, 0, 0, 0),
    "MASSUNIT": (0, 1, 0, 0, 0, 0, 0),
    "TIMEUNIT": (0, 0, 1, 0, 0, 0, 0),
    "PLANEANGLEUNIT": (0, 0, 0, 0, 0, 0, 0),
    "THERMODYNAMICTEMPERATUREUNIT": (0, 0, 0, 0, 1, 0, 0),
    "USERDEFINED": (0, 0, 0, 0, 0, 0, 0),
}


def add_conversion_based_unit(file, name="foot", conversion_offset=None):
    unit_type = ifcopenshell.util.unit.imperial_types.get(name, "USERDEFINED")
    dimensions = ifcopenshell.util.unit.named_dimensions.get(unit_type, (0, 0, 0, 0, 0, 0, 0))
    exponents = file.createIfcDimensionalExponents(*dimensions)
    si_name = ifcopenshell.util.unit.si_type_names.get(unit_type, "METRE")

    if unit_type == "MASSUNIT":
        si_unit = file.createIfcSIUnit(UnitType=unit_type, Name=si_name, Prefix="KILO")
    else:
        si_unit = file.createIfcSIUnit(UnitType=unit_type, Name=si_name)

    conversion_real = ifcopenshell.util.unit.si_conversions.get(name, 1)
    value_component = file.create_entity("IfcReal", wrappedValue=conversion_real)
    conversion_factor = file.createIfcMeasureWithUnit(value_component, si_unit)

    return file.createIfcConversionBasedUnit(exponents, unit_type, name, conversion_factor)
