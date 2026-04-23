# SPDX-License-Identifier: LGPL-3.0-or-later
# This file was generated with the assistance of an AI coding tool.
"""Low-level helpers for the ``ifcopenshell_value_t`` C ABI.

Extracted from ``ifcopenshell.util.selector`` so that modules deep in the
import chain (e.g. :mod:`ifcopenshell.entity_instance`) can reuse the
value marshalling helpers without triggering the larger selector module
import graph, which would cause a circular import.

Only depends on :mod:`ctypes` and the standard library."""
from __future__ import annotations

import ctypes


# ifcopenshell_value_kind_t constants (must match value.h)
IFCSEL_VALUE_NONE     = 0
IFCSEL_VALUE_BOOL     = 1
IFCSEL_VALUE_INT      = 2
IFCSEL_VALUE_DOUBLE   = 3
IFCSEL_VALUE_STRING   = 4
IFCSEL_VALUE_INSTANCE = 5
IFCSEL_VALUE_LIST     = 6
IFCSEL_VALUE_DICT     = 7


_value_lib_configured = False


def configure_value_lib(lib) -> None:
    """Bind ctypes signatures for the value accessors.

    Idempotent; subsequent calls are no-ops."""
    global _value_lib_configured
    if _value_lib_configured:
        return
    lib.ifcopenshell_value_free.restype = None
    lib.ifcopenshell_value_free.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_kind.restype = ctypes.c_int
    lib.ifcopenshell_value_kind.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_bool.restype = ctypes.c_bool
    lib.ifcopenshell_value_as_bool.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_int64.restype = ctypes.c_int64
    lib.ifcopenshell_value_as_int64.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_double.restype = ctypes.c_double
    lib.ifcopenshell_value_as_double.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_string.restype = ctypes.c_char_p
    lib.ifcopenshell_value_as_string.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_instance.restype = ctypes.c_void_p
    lib.ifcopenshell_value_as_instance.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_list_size.restype = ctypes.c_size_t
    lib.ifcopenshell_value_list_size.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_list_at.restype = ctypes.c_void_p
    lib.ifcopenshell_value_list_at.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    lib.ifcopenshell_value_dict_size.restype = ctypes.c_size_t
    lib.ifcopenshell_value_dict_size.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_dict_key_at.restype = ctypes.c_char_p
    lib.ifcopenshell_value_dict_key_at.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    lib.ifcopenshell_value_dict_value_at.restype = ctypes.c_void_p
    lib.ifcopenshell_value_dict_value_at.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    _value_lib_configured = True


def value_to_python(lib, ptr, element):
    """Recursively convert an ``ifcopenshell_value_t*`` to a Python object.

    ``element`` is the originating entity used only to source the file
    handle when materialising nested INSTANCE values as ``entity_instance``
    wrappers."""
    if not ptr:
        return None
    kind = lib.ifcopenshell_value_kind(ptr)
    if kind == IFCSEL_VALUE_NONE:
        return None
    if kind == IFCSEL_VALUE_BOOL:
        return bool(lib.ifcopenshell_value_as_bool(ptr))
    if kind == IFCSEL_VALUE_INT:
        return int(lib.ifcopenshell_value_as_int64(ptr))
    if kind == IFCSEL_VALUE_DOUBLE:
        return float(lib.ifcopenshell_value_as_double(ptr))
    if kind == IFCSEL_VALUE_STRING:
        raw = lib.ifcopenshell_value_as_string(ptr)
        return raw.decode("utf-8", errors="replace") if raw else None
    if kind == IFCSEL_VALUE_INSTANCE:
        h = lib.ifcopenshell_value_as_instance(ptr)
        if not h:
            return None
        # Local import avoids circular dep on entity_instance at module
        # load time.
        from ifcopenshell.entity_instance import entity_instance as _ei
        return _ei(element.file, h)
    if kind == IFCSEL_VALUE_LIST:
        n = lib.ifcopenshell_value_list_size(ptr)
        return [
            value_to_python(lib, lib.ifcopenshell_value_list_at(ptr, i), element)
            for i in range(n)
        ]
    if kind == IFCSEL_VALUE_DICT:
        n = lib.ifcopenshell_value_dict_size(ptr)
        result = {}
        for i in range(n):
            key_raw = lib.ifcopenshell_value_dict_key_at(ptr, i)
            key = key_raw.decode("utf-8", errors="replace") if key_raw else ""
            val = value_to_python(lib, lib.ifcopenshell_value_dict_value_at(ptr, i), element)
            result[key] = val
        return result
    return None
