# SPDX-License-Identifier: LGPL-3.0-or-later

"""Utility functions for IFC units."""

import ctypes
from collections.abc import Generator
from math import pi
from typing import Literal, Optional, Union

import ifcopenshell
import ifcopenshell.ifcopenshell_wrapper as ifcopenshell_wrapper
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import entity_instance


prefixes = {
    "EXA": 1e18, "PETA": 1e15, "TERA": 1e12, "GIGA": 1e9,
    "MEGA": 1e6, "KILO": 1e3, "HECTO": 1e2, "DECA": 1e1,
    "DECI": 1e-1, "CENTI": 1e-2, "MILLI": 1e-3, "MICRO": 1e-6,
    "NANO": 1e-9, "PICO": 1e-12, "FEMTO": 1e-15, "ATTO": 1e-18,
}

unit_names = [
    "AMPERE", "BECQUEREL", "CANDELA", "COULOMB", "CUBIC_METRE",
    "DEGREE_CELSIUS", "FARAD", "GRAM", "GRAY", "HENRY", "HERTZ",
    "JOULE", "KELVIN", "LUMEN", "LUX", "MOLE", "NEWTON", "OHM",
    "PASCAL", "RADIAN", "SECOND", "SIEMENS", "SIEVERT",
    "SQUARE_METRE", "METRE", "STERADIAN", "TESLA", "VOLT", "WATT", "WEBER",
]

si_dimensions = {
    "METRE": (1, 0, 0, 0, 0, 0, 0),
    "SQUARE_METRE": (2, 0, 0, 0, 0, 0, 0),
    "CUBIC_METRE": (3, 0, 0, 0, 0, 0, 0),
    "GRAM": (0, 1, 0, 0, 0, 0, 0),
    "SECOND": (0, 0, 1, 0, 0, 0, 0),
    "AMPERE": (0, 0, 0, 1, 0, 0, 0),
    "KELVIN": (0, 0, 0, 0, 1, 0, 0),
    "MOLE": (0, 0, 0, 0, 0, 1, 0),
    "CANDELA": (0, 0, 0, 0, 0, 0, 1),
    "RADIAN": (0, 0, 0, 0, 0, 0, 0),
    "STERADIAN": (0, 0, 0, 0, 0, 0, 0),
    "HERTZ": (0, 0, -1, 0, 0, 0, 0),
    "NEWTON": (1, 1, -2, 0, 0, 0, 0),
    "PASCAL": (-1, 1, -2, 0, 0, 0, 0),
    "JOULE": (2, 1, -2, 0, 0, 0, 0),
    "WATT": (2, 1, -3, 0, 0, 0, 0),
    "COULOMB": (0, 0, 1, 1, 0, 0, 0),
    "VOLT": (2, 1, -3, -1, 0, 0, 0),
    "FARAD": (-2, -1, 4, 2, 0, 0, 0),
    "OHM": (2, 1, -3, -2, 0, 0, 0),
    "SIEMENS": (-2, -1, 3, 2, 0, 0, 0),
    "WEBER": (2, 1, -2, -1, 0, 0, 0),
    "TESLA": (0, 1, -2, -1, 0, 0, 0),
    "HENRY": (2, 1, -2, -2, 0, 0, 0),
    "DEGREE_CELSIUS": (0, 0, 0, 0, 1, 0, 0),
    "LUMEN": (0, 0, 0, 0, 0, 0, 1),
    "LUX": (-2, 0, 0, 0, 0, 0, 1),
    "BECQUEREL": (0, 0, -1, 0, 0, 0, 0),
    "GRAY": (2, 0, -2, 0, 0, 0, 0),
    "SIEVERT": (2, 0, -2, 0, 0, 0, 0),
    "OTHERWISE": (0, 0, 0, 0, 0, 0, 0),
}

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

named_dimensions = {
    "ABSORBEDDOSEUNIT": (2, 0, -2, 0, 0, 0, 0),
    "AMOUNTOFSUBSTANCEUNIT": (0, 0, 0, 0, 0, 1, 0),
    "AREAUNIT": (2, 0, 0, 0, 0, 0, 0),
    "DOSEEQUIVALENTUNIT": (2, 0, -2, 0, 0, 0, 0),
    "ELECTRICCAPACITANCEUNIT": (-2, -1, 4, 2, 0, 0, 0),
    "ELECTRICCHARGEUNIT": (0, 0, 1, 1, 0, 0, 0),
    "ELECTRICCONDUCTANCEUNIT": (-2, -1, 3, 2, 0, 0, 0),
    "ELECTRICCURRENTUNIT": (0, 0, 0, 1, 0, 0, 0),
    "ELECTRICRESISTANCEUNIT": (2, 1, -3, -2, 0, 0, 0),
    "ELECTRICVOLTAGEUNIT": (2, 1, -3, -1, 0, 0, 0),
    "ENERGYUNIT": (2, 1, -2, 0, 0, 0, 0),
    "FORCEUNIT": (1, 1, -2, 0, 0, 0, 0),
    "FREQUENCYUNIT": (0, 0, -1, 0, 0, 0, 0),
    "ILLUMINANCEUNIT": (-2, 0, 0, 0, 0, 1, 1),
    "INDUCTANCEUNIT": (2, 1, -2, -2, 0, 0, 0),
    "LENGTHUNIT": (1, 0, 0, 0, 0, 0, 0),
    "LUMINOUSFLUXUNIT": (0, 0, 0, 0, 0, 1, 1),
    "LUMINOUSINTENSITYUNIT": (0, 0, 0, 0, 0, 0, 1),
    "MAGNETICFLUXDENSITYUNIT": (0, 1, -2, -1, 0, 0, 0),
    "MAGNETICFLUXUNIT": (2, 1, -2, -1, 0, 0, 0),
    "MASSUNIT": (0, 1, 0, 0, 0, 0, 0),
    "PLANEANGLEUNIT": (0, 0, 0, 0, 0, 0, 0),
    "POWERUNIT": (2, 1, -3, 0, 0, 0, 0),
    "PRESSUREUNIT": (-1, 1, -2, 0, 0, 0, 0),
    "RADIOACTIVITYUNIT": (0, 0, -1, 0, 0, 0, 0),
    "SOLIDANGLEUNIT": (0, 0, 0, 0, 0, 0, 0),
    "THERMODYNAMICTEMPERATUREUNIT": (0, 0, 0, 0, 1, 0, 0),
    "TIMEUNIT": (0, 0, 1, 0, 0, 0, 0),
    "VOLUMEUNIT": (3, 0, 0, 0, 0, 0, 0),
    "USERDEFINED": (0, 0, 0, 0, 0, 0, 0),
}

si_conversions = {
    "thou": 0.0000254, "inch": 0.0254, "foot": 0.3048,
    "yard": 0.914, "mile": 1609,
    "square thou": 6.4516e-10, "square inch": 0.0006452,
    "square foot": 0.09290304, "square yard": 0.83612736,
    "acre": 4046.86, "square mile": 2588881,
    "cubic thou": 1.6387064e-14, "cubic inch": 0.00001639,
    "cubic foot": 0.02831684671168849, "cubic yard": 0.7636,
    "cubic mile": 4165509529, "litre": 0.001,
    "fluid ounce UK": 0.0000284130625, "fluid ounce US": 0.00002957353,
    "pint UK": 0.000568, "pint US": 0.000473,
    "gallon UK": 0.004546, "gallon US": 0.003785,
    "degree": pi / 180,
    "ounce": 0.02835, "pound": 0.454,
    "ton UK": 1016.0469088, "ton US": 907.18474, "tonne": 1000.0,
    "lbf": 4.4482216153, "kip": 4448.2216153,
    "psi": 6894.7572932, "ksi": 6894757.2932,
    "minute": 60, "hour": 3600, "day": 86400,
    "btu": 1055.056, "fahrenheit": 1.8,
}

si_offsets = {
    "fahrenheit": -459.67,
}

imperial_types = {
    "thou": "LENGTHUNIT", "inch": "LENGTHUNIT", "foot": "LENGTHUNIT",
    "yard": "LENGTHUNIT", "mile": "LENGTHUNIT",
    "square thou": "AREAUNIT", "square inch": "AREAUNIT",
    "square foot": "AREAUNIT", "square yard": "AREAUNIT",
    "acre": "AREAUNIT", "square mile": "AREAUNIT",
    "cubic thou": "VOLUMEUNIT", "cubic inch": "VOLUMEUNIT",
    "cubic foot": "VOLUMEUNIT", "cubic yard": "VOLUMEUNIT",
    "cubic mile": "VOLUMEUNIT", "litre": "VOLUMEUNIT",
    "fluid ounce UK": "VOLUMEUNIT", "fluid ounce US": "VOLUMEUNIT",
    "pint UK": "VOLUMEUNIT", "pint US": "VOLUMEUNIT",
    "gallon UK": "VOLUMEUNIT", "gallon US": "VOLUMEUNIT",
    "degree": "PLANEANGLEUNIT",
    "ounce": "MASSUNIT", "pound": "MASSUNIT",
    "ton UK": "MASSUNIT", "ton US": "MASSUNIT", "tonne": "MASSUNIT",
    "lbf": "FORCEUNIT", "kip": "FORCEUNIT",
    "psi": "PRESSUREUNIT", "ksi": "PRESSUREUNIT",
    "minute": "TIMEUNIT", "hour": "TIMEUNIT", "day": "TIMEUNIT",
    "btu": "ENERGYUNIT",
    "fahrenheit": "THERMODYNAMICTEMPERATUREUNIT",
}

prefix_symbols = {
    "EXA": "E", "PETA": "P", "TERA": "T", "GIGA": "G",
    "MEGA": "M", "KILO": "k", "HECTO": "h", "DECA": "da",
    "DECI": "d", "CENTI": "c", "MILLI": "m", "MICRO": "μ",
    "NANO": "n", "PICO": "p", "FEMTO": "f", "ATTO": "a",
}

unit_symbols = {
    "CUBIC_METRE": "m3", "GRAM": "g", "SECOND": "s",
    "SQUARE_METRE": "m2", "METRE": "m", "NEWTON": "N", "PASCAL": "Pa",
    "pound-force": "lbf", "pound-force per square inch": "psi",
    "thou": "th", "inch": "in", "foot": "ft", "yard": "yd", "mile": "mi",
    "square thou": "th2", "square inch": "in2", "square foot": "ft2",
    "square yard": "yd2", "acre": "ac", "square mile": "mi2",
    "cubic thou": "th3", "cubic inch": "in3", "cubic foot": "ft3",
    "cubic yard": "yd3", "cubic mile": "mi3", "litre": "L",
    "fluid ounce UK": "fl oz", "fluid ounce US": "fl oz",
    "pint UK": "pt", "pint US": "pt",
    "gallon UK": "gal", "gallon US": "gal",
    "degree": "°",
    "ounce": "oz", "pound": "lb",
    "ton UK": "ton", "ton US": "ton", "tonne": "t",
    "lbf": "lbf", "kip": "kip", "psi": "psi", "ksi": "ksi",
    "minute": "min", "hour": "hr", "day": "day",
    "btu": "btu", "fahrenheit": "°F",
}

QUANTITY_CLASS = Literal[
    "IfcQuantityCount",
    "IfcQuantityNumber",
    "IfcQuantityLength",
    "IfcQuantityArea",
    "IfcQuantityVolume",
    "IfcQuantityWeight",
    "IfcQuantityTime",
    "IfcQuantityCount",
]

MEASURE_CLASS = Literal[
    "IfcNumericMeasure",
    "IfcLengthMeasure",
    "IfcAreaMeasure",
    "IfcVolumeMeasure",
    "IfcMassMeasure",
]


# ---------------------------------------------------------------------------
# ctypes binding to libifcopenshell_capi.
# ---------------------------------------------------------------------------

_lib_configured = False


def _configure(lib) -> None:
    global _lib_configured
    if _lib_configured:
        return
    cp = ctypes.c_char_p
    vp = ctypes.c_void_p

    _generated_capi.bind(
        lib,
        names=(
            "ifcopenshell_ifcapi_unit_get_prefix",
            "ifcopenshell_ifcapi_unit_get_prefix_multiplier",
            "ifcopenshell_ifcapi_unit_get_unit_name",
            "ifcopenshell_ifcapi_unit_get_unit_name_universal",
            "ifcopenshell_ifcapi_unit_get_measure_class",
            "ifcopenshell_ifcapi_unit_get_measure_unit_type",
            "ifcopenshell_ifcapi_unit_get_symbol_measure_class",
            "ifcopenshell_ifcapi_unit_get_symbol_quantity_class",
            "ifcopenshell_ifcapi_unit_get_si_dimensions",
            "ifcopenshell_ifcapi_unit_get_named_dimensions",
            "ifcopenshell_ifcapi_unit_convert",
            "ifcopenshell_ifcapi_unit_format_length",
            "ifcopenshell_string_destroy",
            "ifcopenshell_int32_list_destroy",
        ),
    )

    lib.ifcopenshell_util_unit_get_unit_assignment.restype = vp
    lib.ifcopenshell_util_unit_get_unit_assignment.argtypes = [vp]
    lib.ifcopenshell_util_unit_get_project_unit.restype = vp
    lib.ifcopenshell_util_unit_get_project_unit.argtypes = [vp, cp]
    lib.ifcopenshell_util_unit_get_full_unit_name.restype = vp
    lib.ifcopenshell_util_unit_get_full_unit_name.argtypes = [vp]
    lib.ifcopenshell_util_unit_get_unit_symbol.restype = vp
    lib.ifcopenshell_util_unit_get_unit_symbol.argtypes = [vp]
    lib.ifcopenshell_util_unit_convert_unit.restype = ctypes.c_double
    lib.ifcopenshell_util_unit_convert_unit.argtypes = [ctypes.c_double, vp, vp]
    lib.ifcopenshell_util_unit_resolve_property.restype = None
    lib.ifcopenshell_util_unit_resolve_property.argtypes = [
        vp, ctypes.POINTER(vp), ctypes.POINTER(vp),
    ]
    lib.ifcopenshell_util_unit_resolve_property_table.restype = None
    lib.ifcopenshell_util_unit_resolve_property_table.argtypes = [
        vp, ctypes.POINTER(vp), ctypes.POINTER(vp),
        ctypes.POINTER(vp), ctypes.POINTER(vp),
    ]

    lib.ifcopenshell_util_unit_calculate_unit_scale.restype = ctypes.c_double
    lib.ifcopenshell_util_unit_calculate_unit_scale.argtypes = [vp, cp]

    _lib_configured = True


def _enc(s):
    if s is None:
        return None
    if isinstance(s, bytes):
        return s
    return s.encode("utf-8")


def _free_str(lib, ptr):
    if ptr:
        lib.ifcopenshell_free_string(ctypes.cast(ptr, ctypes.c_char_p))


def _take_str(lib, ptr):
    if not ptr:
        return None
    s = ctypes.string_at(ptr).decode("utf-8")
    _free_str(lib, ptr)
    return s


def _take_generated_str(lib, out):
    try:
        if not out.data:
            return ""
        return ctypes.string_at(out.data, out.size).decode("utf-8")
    finally:
        lib.ifcopenshell_string_destroy(ctypes.byref(out))


def _call_generated_string(lib, name, value):
    out = _generated_capi.ifcopenshell_string_t()
    if not getattr(lib, name)(_enc(value) or b"", ctypes.byref(out)):
        raise RuntimeError(ifcopenshell.get_log() or name)
    return _take_generated_str(lib, out)


def _call_generated_int_tuple(lib, name, value):
    out = _generated_capi.ifcopenshell_int32_list_t()
    if not getattr(lib, name)(_enc(value) or b"", ctypes.byref(out)):
        raise RuntimeError(ifcopenshell.get_log() or name)
    try:
        return tuple(out.items[i] for i in range(out.length))
    finally:
        lib.ifcopenshell_int32_list_destroy(ctypes.byref(out))


def _take_instance(lib, file_obj, handle):
    if not handle:
        return None
    return entity_instance(file_obj, handle)


def _file_ptr(ifc_file):
    return getattr(ifc_file, "_ptr", None)


# ---------------------------------------------------------------------------
# Pure-string helpers.
# ---------------------------------------------------------------------------

def get_prefix(text):
    if not text:
        return None
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_prefix", text) or None


def get_prefix_multiplier(text):
    lib = ifcopenshell._get_lib()
    _configure(lib)
    out = ctypes.c_double()
    if not lib.ifcopenshell_ifcapi_unit_get_prefix_multiplier(_enc(text) or b"", ctypes.byref(out)):
        raise RuntimeError(ifcopenshell.get_log() or "ifcopenshell_ifcapi_unit_get_prefix_multiplier")
    return out.value


def get_unit_name(text: str) -> Union[str, None]:
    """Get unit name from str, if unit is in SI."""
    if text is None:
        return None
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_unit_name", text) or None


def get_unit_name_universal(text: str) -> Union[str, None]:
    if text is None:
        return None
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_unit_name_universal", text) or None


def get_si_dimensions(name):
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_int_tuple(lib, "ifcopenshell_ifcapi_unit_get_si_dimensions", name)


def get_named_dimensions(name):
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_int_tuple(lib, "ifcopenshell_ifcapi_unit_get_named_dimensions", name)


def get_unit_measure_class(unit_type: str) -> MEASURE_CLASS:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_measure_class", unit_type)


def get_measure_unit_type(measure_class: MEASURE_CLASS) -> str:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_measure_unit_type", measure_class)


def get_symbol_measure_class(symbol: Optional[str] = None) -> MEASURE_CLASS:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_symbol_measure_class", symbol or "")


def get_symbol_quantity_class(symbol: Optional[str] = None) -> QUANTITY_CLASS:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return _call_generated_string(lib, "ifcopenshell_ifcapi_unit_get_symbol_quantity_class", symbol or "")


def convert(value: float, from_prefix: Optional[str], from_unit: str,
            to_prefix: Optional[str], to_unit: str) -> float:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    out = ctypes.c_double()
    if not lib.ifcopenshell_ifcapi_unit_convert(
        float(value), _enc(from_prefix) or b"", _enc(from_unit) or b"",
        _enc(to_prefix) or b"", _enc(to_unit) or b"", ctypes.byref(out),
    ):
        raise RuntimeError(ifcopenshell.get_log() or "ifcopenshell_ifcapi_unit_convert")
    return out.value


def format_length(
    value: float,
    precision: float,
    decimal_places: int = 2,
    suppress_zero_inches: bool = True,
    unit_system: Literal["metric", "imperial"] = "imperial",
    input_unit: Literal["foot", "inch"] = "foot",
    output_unit: Literal["foot", "inch"] = "foot",
) -> str:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    out = _generated_capi.ifcopenshell_string_t()
    if not lib.ifcopenshell_ifcapi_unit_format_length(
        float(value), float(precision), int(decimal_places), bool(suppress_zero_inches),
        _enc(unit_system) or b"", _enc(input_unit) or b"", _enc(output_unit) or b"", ctypes.byref(out),
    ):
        raise RuntimeError(ifcopenshell.get_log() or "ifcopenshell_ifcapi_unit_format_length")
    result = _take_generated_str(lib, out)
    return result or None


# ---------------------------------------------------------------------------
# Entity-based helpers.
# ---------------------------------------------------------------------------

def get_unit_assignment(ifc_file) -> Union[entity_instance, None]:
    if ifc_file is None:
        return None
    lib = ifcopenshell._get_lib()
    _configure(lib)
    h = lib.ifcopenshell_util_unit_get_unit_assignment(_file_ptr(ifc_file))
    return _take_instance(lib, ifc_file, h)


def cache_units(ifc_file) -> None:
    """Cache the default units for performance.

    Repetitively fetching project units (such as for determining the unit of a
    property) can be costly. This enables a cache to make it faster. If the
    project units change, you can update the cache by rerunning this function.
    """
    ifc_file.units = {}
    if assignment := get_unit_assignment(ifc_file):
        ifc_file.units = {
            u.UnitType: u
            for u in (assignment.Units or [])
            if getattr(u, "UnitType", None)
        }


def clear_unit_cache(ifc_file) -> None:
    """Clears the unit cache of the project."""
    ifc_file.units = {}


def get_project_unit(ifc_file, unit_type: str, use_cache: bool = False) -> Union[entity_instance, None]:
    """Get the default project unit of a particular unit type."""
    if use_cache and not ifc_file.units:
        cache_units(ifc_file)
    if units := ifc_file.units:
        return units.get(unit_type, None)
    lib = ifcopenshell._get_lib()
    _configure(lib)
    h = lib.ifcopenshell_util_unit_get_project_unit(_file_ptr(ifc_file), _enc(unit_type))
    return _take_instance(lib, ifc_file, h)


def get_full_unit_name(unit) -> str:
    if unit is None:
        return ""
    lib = ifcopenshell._get_lib()
    _configure(lib)
    p = lib.ifcopenshell_util_unit_get_full_unit_name(unit._handle)
    return _take_str(lib, p) or ""


def get_unit_symbol(unit) -> str:
    if unit is None:
        return ""
    lib = ifcopenshell._get_lib()
    _configure(lib)
    p = lib.ifcopenshell_util_unit_get_unit_symbol(unit._handle)
    return _take_str(lib, p) or ""


def convert_unit(value: float, from_unit, to_unit) -> float:
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return lib.ifcopenshell_util_unit_convert_unit(
        float(value),
        from_unit._handle if from_unit is not None else None,
        to_unit._handle if to_unit is not None else None,
    )


def get_property_unit(prop, ifc_file, use_cache: bool = False) -> Union[entity_instance, None]:
    """Resolve the unit of a property/quantity, falling back to project default."""
    if prop is None:
        return None
    lib = ifcopenshell._get_lib()
    _configure(lib)
    out_unit = ctypes.c_void_p(0)
    out_mc = ctypes.c_void_p(0)
    lib.ifcopenshell_util_unit_resolve_property(
        prop._handle, ctypes.byref(out_unit), ctypes.byref(out_mc),
    )
    target = ifc_file if ifc_file is not None else prop.file
    if out_unit.value:
        if out_mc.value:
            _free_str(lib, out_mc.value)
        return entity_instance(target, out_unit.value)
    if not out_mc.value:
        return None
    measure_class = _take_str(lib, out_mc.value)
    unit_type = get_measure_unit_type(measure_class)
    if not unit_type:
        return None
    return get_project_unit(target, unit_type, use_cache=use_cache)


def get_property_table_unit(prop, ifc_file, use_cache: bool = False) -> dict:
    if prop is None:
        return {"DefiningUnit": None, "DefinedUnit": None}
    lib = ifcopenshell._get_lib()
    _configure(lib)
    defining_h = ctypes.c_void_p(0)
    defining_mc = ctypes.c_void_p(0)
    defined_h = ctypes.c_void_p(0)
    defined_mc = ctypes.c_void_p(0)
    lib.ifcopenshell_util_unit_resolve_property_table(
        prop._handle,
        ctypes.byref(defining_h), ctypes.byref(defining_mc),
        ctypes.byref(defined_h), ctypes.byref(defined_mc),
    )
    target = ifc_file if ifc_file is not None else prop.file

    def _resolve(unit_handle, mc_handle):
        if unit_handle.value:
            if mc_handle.value:
                _free_str(lib, mc_handle.value)
            return entity_instance(target, unit_handle.value)
        if not mc_handle.value:
            return None
        mc = _take_str(lib, mc_handle.value)
        ut = get_measure_unit_type(mc)
        return get_project_unit(target, ut, use_cache=use_cache) if ut else None

    return {
        "DefiningUnit": _resolve(defining_h, defining_mc),
        "DefinedUnit": _resolve(defined_h, defined_mc),
    }


def calculate_unit_scale(ifc_file, unit_type: str = "LENGTHUNIT") -> float:
    """Returns a unit scale factor to convert to and from IFC project units and SI units."""
    if (
        type(ifc_file) is ifcopenshell.file
        and unit_type
        not in ifcopenshell.ifcopenshell_wrapper.schema_by_name(ifc_file.schema_identifier)
        .declaration_by_name("IfcUnitEnum")
        .enumeration_items()
    ):
        raise ValueError(f"Unit type {unit_type!r} does not name a valid type")
    lib = ifcopenshell._get_lib()
    _configure(lib)
    return lib.ifcopenshell_util_unit_calculate_unit_scale(_file_ptr(ifc_file), _enc(unit_type))


# ---------------------------------------------------------------------------
# Schema-introspection helpers (kept in Python).
# ---------------------------------------------------------------------------

def is_attr_type(
    content_type,
    ifc_unit_type_name: str,
    include_select_types: bool = True,
):
    cur_decl = content_type

    if hasattr(cur_decl, "name") and cur_decl.name() == ifc_unit_type_name:
        return cur_decl

    if include_select_types:
        if hasattr(cur_decl, "select_list"):
            for select_item in cur_decl.select_list():
                if is_attr_type(select_item, ifc_unit_type_name):
                    return select_item

    if hasattr(cur_decl, "declared_type"):
        return is_attr_type(cur_decl.declared_type(), ifc_unit_type_name, include_select_types)

    if isinstance(cur_decl, ifcopenshell_wrapper.aggregation_type):
        def get_declared_type_from_aggregate(cur_decl):
            cur_decl = cur_decl.type_of_element()
            if not isinstance(cur_decl, ifcopenshell_wrapper.aggregation_type):
                return cur_decl.declared_type()
            return get_declared_type_from_aggregate(cur_decl)

        cur_decl = get_declared_type_from_aggregate(cur_decl)
        return is_attr_type(cur_decl, ifc_unit_type_name, include_select_types)

    return None


FloatOrSequenceOfFloats = Union[float, tuple]


def iter_element_and_attributes_per_type(ifc_file, attr_type_name: str) -> Generator:
    schema = ifcopenshell_wrapper.schema_by_name(ifc_file.schema_identifier)

    for element in ifc_file:
        entity = schema.declaration_by_name(element.is_a()).as_entity()
        assert entity
        attrs = entity.all_attributes()
        attrs_derived = entity.derived()
        for attr, val, is_derived in zip(attrs, list(element), attrs_derived):
            if is_derived:
                continue

            attr_type = attr.type_of_attribute()
            base_type = is_attr_type(attr_type, attr_type_name)
            if base_type is None:
                continue

            if val is None:
                continue

            if isinstance(val, (ifcopenshell.entity_instance, ifcopenshell._typed_value)) and not val.is_a(attr_type_name):
                continue
            elif isinstance(val, tuple):
                if not val:
                    continue
                val_ = val[0]
                if isinstance(val_, (ifcopenshell.entity_instance, ifcopenshell._typed_value)):
                    for val_ in val:
                        if not val_.is_a(attr_type_name):
                            continue
                        yield element, attr, val_
                    continue

            yield element, attr, val


def convert_file_length_units(ifc_file, target_units: str = "METER"):
    """Converts all units in an IFC file to the specified target units. Returns a new file."""
    import ifcopenshell.api.georeference
    import ifcopenshell.api.unit
    import ifcopenshell.util.element
    import ifcopenshell.util.geolocation

    prefix = get_prefix(target_units)
    si_unit = get_unit_name(target_units)

    file_patched = ifcopenshell.file.from_string(ifc_file.to_string())

    old_length = get_project_unit(file_patched, "LENGTHUNIT")
    if si_unit:
        new_length = ifcopenshell.api.unit.add_si_unit(file_patched, unit_type="LENGTHUNIT", prefix=prefix)
    else:
        target_units = target_units.lower()
        if imperial_types.get(target_units) != "LENGTHUNIT":
            raise Exception(
                f'Couldn\'t identify target units "{target_units}". '
                'The method supports singular unit names like "CENTIMETER", "METER", "FOOT", etc.'
            )
        new_length = ifcopenshell.api.unit.add_conversion_based_unit(file_patched, name=target_units)

    def convert_value(value):
        if not isinstance(value, tuple):
            return convert_unit(value, old_length, new_length)
        return tuple(convert_value(v) for v in value)

    def _convert_typed(c):
        if isinstance(c, ifcopenshell._typed_value):
            if c.is_a("IfcLengthMeasure"):
                return ifcopenshell._typed_value(file_patched, c.is_a(), str(convert_value(c.wrappedValue)))
            return c
        if isinstance(c, ifcopenshell.entity_instance):
            if c.is_a("IfcLengthMeasure"):
                return ifcopenshell._typed_value(file_patched, c.is_a(), str(convert_value(c.wrappedValue)))
            return c
        if isinstance(c, (list, tuple)):
            return type(c)(_convert_typed(x) for x in c)
        return convert_value(c)

    seen: set = set()
    for element, attr, val in iter_element_and_attributes_per_type(file_patched, "IfcLengthMeasure"):
        key = (element.id(), attr.name())
        if key in seen:
            continue
        seen.add(key)
        current = getattr(element, attr.name())
        setattr(element, attr.name(), _convert_typed(current))

    has_map_unit = False
    if (
        ifc_file.schema == "IFC2X3"
        and (crs := ifcopenshell.util.element.get_pset(ifc_file.by_type("IfcProject")[0], name="ePSet_ProjectedCRS"))
        and crs.get("MapUnit")
    ) or (ifc_file.schema != "IFC2X3" and (crs := ifc_file.by_type("IfcProjectedCRS")) and crs[0].MapUnit):
        has_map_unit = True

    if has_map_unit:
        parameters = ifcopenshell.util.geolocation.get_helmert_transformation_parameters(ifc_file)
        ifcopenshell.api.georeference.edit_georeferencing(
            file_patched,
            coordinate_operation={
                "Eastings": parameters.e,
                "Northings": parameters.n,
                "OrthogonalHeight": parameters.h,
                "Scale": parameters.scale / convert_value(1),
            },
        )

    unit_assignment = get_unit_assignment(file_patched)
    unit_assignment.Units = [new_length, *(u for u in unit_assignment.Units if u.UnitType != new_length.UnitType)]
    if not file_patched.get_total_inverses(old_length):
        ifcopenshell.util.element.remove_deep2(file_patched, old_length)

    return file_patched
