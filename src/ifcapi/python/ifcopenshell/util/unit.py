# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Utility functions for IFC units."""

import ifcopenshell

si_type_names = {
    "ABSORBEDDOSEUNIT": "GRAY",
    "AMOUNTOFSUBSTANCEUNIT": "MOLE",
    "AREAUNIT": "SQUARE_METRE",
    "DOSEEQUIVALENTUNIT": "SIEVERT",
    "ELECTRICCAPACITANCEUNIT": "FARAD",
    "ELECTRICCHARGEUNIT": "COULOMB",
    "ELECTRICCONDUCTANCEUNIT": "SIEMENS",
    "ELECTRICCURRENTUNIT": "AMPERE",
    "ELECTRICRESISTANCEUNIT": "OHM",
    "ELECTRICVOLTAGEUNIT": "VOLT",
    "ENERGYUNIT": "JOULE",
    "FORCEUNIT": "NEWTON",
    "FREQUENCYUNIT": "HERTZ",
    "ILLUMINANCEUNIT": "LUX",
    "INDUCTANCEUNIT": "HENRY",
    "LENGTHUNIT": "METRE",
    "LUMINOUSFLUXUNIT": "LUMEN",
    "LUMINOUSINTENSITYUNIT": "CANDELA",
    "MAGNETICFLUXDENSITYUNIT": "TESLA",
    "MAGNETICFLUXUNIT": "WEBER",
    "MASSUNIT": "GRAM",
    "PLANEANGLEUNIT": "RADIAN",
    "POWERUNIT": "WATT",
    "PRESSUREUNIT": "PASCAL",
    "RADIOACTIVITYUNIT": "BECQUEREL",
    "SOLIDANGLEUNIT": "STERADIAN",
    "THERMODYNAMICTEMPERATUREUNIT": "KELVIN",
    "TIMEUNIT": "SECOND",
    "VOLUMEUNIT": "CUBIC_METRE",
    "USERDEFINED": "METRE",
}

prefixes = {
    "EXA": 1e18, "PETA": 1e15, "TERA": 1e12, "GIGA": 1e9, "MEGA": 1e6,
    "KILO": 1e3, "HECTO": 1e2, "DECA": 1e1,
    "DECI": 1e-1, "CENTI": 1e-2, "MILLI": 1e-3, "MICRO": 1e-6,
    "NANO": 1e-9, "PICO": 1e-12, "FEMTO": 1e-15, "ATTO": 1e-18,
}

unit_names = [
    "CUBIC_METRE", "SQUARE_METRE", "METRE",
    "GRAM", "SECOND", "AMPERE", "KELVIN", "MOLE", "CANDELA",
    "RADIAN", "STERADIAN", "HERTZ", "NEWTON", "PASCAL", "JOULE",
    "WATT", "COULOMB", "VOLT", "FARAD", "OHM", "SIEMENS",
    "WEBER", "TESLA", "HENRY", "DEGREE_CELSIUS", "LUMEN", "LUX",
    "BECQUEREL", "GRAY", "SIEVERT",
]

si_conversions = {
    "thou": 0.0000254,
    "inch": 0.0254,
    "foot": 0.3048,
    "yard": 0.914,
    "mile": 1609,
    "square thou": 6.4516e-10,
    "square inch": 0.0006452,
    "square foot": 0.09290304,
    "square yard": 0.83612736,
    "square mile": 2588881,
    "cubic thou": 1.6387064e-14,
    "cubic inch": 0.00001639,
    "cubic foot": 0.02831684671168849,
    "cubic yard": 0.7636,
    "cubic mile": 4165509529,
    "litre": 0.001,
}


def get_prefix(text):
    if text:
        for prefix in prefixes.keys():
            if prefix in text.upper():
                return prefix


def get_prefix_multiplier(text):
    if not text:
        return 1
    prefix = get_prefix(text)
    if prefix:
        return prefixes[prefix]
    return 1


def get_unit_name(text):
    text = text.upper().replace("METER", "METRE")
    for name in unit_names:
        if name.replace("_", " ") in text:
            return name


def get_unit_assignment(ifc_file):
    projects = ifc_file.by_type("IfcProject")
    if not projects:
        return None
    return projects[0].UnitsInContext


def calculate_unit_scale(ifc_file, unit_type="LENGTHUNIT"):
    """Return a scale factor: ifc_project_length * scale = SI metres."""
    if not (projects := ifc_file.by_type("IfcProject")) or not (units := projects[0].UnitsInContext):
        return 1
    unit_scale = 1
    for unit in units.Units:
        if getattr(unit, "UnitType", ...) != unit_type:
            continue
        while unit.is_a("IfcConversionBasedUnit"):
            conversion_factor = unit.ConversionFactor
            unit_scale *= conversion_factor.ValueComponent.wrappedValue
            unit = conversion_factor.UnitComponent
        if unit.is_a("IfcSIUnit"):
            unit_scale *= get_prefix_multiplier(unit.Prefix)
    return unit_scale


def get_project_unit(ifc_file, unit_type):
    """Get the default project unit of a particular unit type."""
    if unit_assignment := get_unit_assignment(ifc_file):
        for unit in unit_assignment.Units or []:
            if getattr(unit, "UnitType", None) == unit_type:
                return unit


def convert(value, from_prefix, from_unit, to_prefix, to_unit):
    """Convert between length, area, and volume units."""
    if from_unit.lower() in si_conversions:
        value *= si_conversions[from_unit.lower()]
    elif from_prefix:
        value *= get_prefix_multiplier(from_prefix)
        if "SQUARE" in from_unit:
            value *= get_prefix_multiplier(from_prefix)
        elif "CUBIC" in from_unit:
            value *= get_prefix_multiplier(from_prefix)
            value *= get_prefix_multiplier(from_prefix)
    if to_unit.lower() in si_conversions:
        return value * (1 / si_conversions[to_unit.lower()])
    elif to_prefix:
        value *= 1 / get_prefix_multiplier(to_prefix)
        if "SQUARE" in from_unit:
            value *= 1 / get_prefix_multiplier(to_prefix)
        elif "CUBIC" in from_unit:
            value *= 1 / get_prefix_multiplier(to_prefix)
            value *= 1 / get_prefix_multiplier(to_prefix)
    return value


def get_unit_measure_class(unit_type):
    """Get the IFC measure class for a unit type (e.g. LENGTHUNIT -> IfcLengthMeasure)."""
    if unit_type == "USERDEFINED":
        return "IfcNumericMeasure"
    return "Ifc" + unit_type[0:-4].lower().capitalize() + "Measure"


from fractions import Fraction
try:
    from typing import Literal
except ImportError:
    Literal = None


def format_length(
    value,
    precision,
    decimal_places=2,
    suppress_zero_inches=True,
    unit_system="imperial",
    input_unit="foot",
    output_unit="foot",
):
    """Formats a length for readability and imperial formatting.

    Ported from the upstream ifcopenshell.util.unit implementation so that
    selector's metric_length / imperial_length format helpers continue to
    work when using the native-backed ifcopenshell package.
    """
    if unit_system == "imperial":
        if input_unit == "foot":
            feet = int(value)
            inches = (value - feet) * 12
        elif input_unit == "inch":
            inches = value % 12
            feet = int(round((value - inches) / 12))

        nearest = round(inches * precision)
        frac = Fraction(nearest, precision)

        if frac.denominator == 1:
            if suppress_zero_inches and frac.numerator == 0:
                if output_unit == "foot":
                    return f"{feet}'"
                return f'{feet * 12}"'
            if output_unit == "foot":
                return f"{feet}' - {frac.numerator}\""
            return f'{(feet * 12) + frac.numerator}"'
        if frac.numerator > frac.denominator:
            remainder = frac.numerator % frac.denominator
            whole = int((frac.numerator - remainder) / frac.denominator)
            if output_unit == "foot":
                return f"{feet}' - {whole} {remainder}/{frac.denominator}\""
            return f'{(feet * 12) + whole} {remainder}/{frac.denominator}"'
        if output_unit == "foot":
            return f"{feet}' - 0 {frac.numerator}/{frac.denominator}\""
        return f'{feet * 12} {frac.numerator}/{frac.denominator}"'
    elif unit_system == "metric":
        rounded_val = round(value / precision) * precision
        return f"{rounded_val:.{decimal_places}f}"
