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
import sys

from ifcopenshell import _generated_capi


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
    """Bind ctypes signatures for the generated value facade.

    Idempotent; subsequent calls are no-ops."""
    global _value_lib_configured
    if _value_lib_configured:
        return
    _generated_capi.bind(
        lib,
        names=("ifcopenshell_ifcapi_compute_derived",),
        prefixes=("ifcopenshell_ifcapi_value_",),
    )
    _value_lib_configured = True


def _take_string(lib, fn, *args):
    from ifcopenshell import ifcopenshell_wrapper as W

    out = W.ifcopenshell_string_t()
    if not fn(*args, ctypes.byref(out)):
        return None
    return W._take_string(out)


def _take_scalar(lib, fn, c_type, ptr):
    out = c_type()
    if not fn(ptr, ctypes.byref(out)):
        return None
    return out.value


def value_to_python(lib, ptr, element):
    """Recursively convert an ``ifcopenshell_value_t*`` to a Python object.

    ``element`` is the originating entity used only to source the file
    handle when materialising nested INSTANCE values as ``entity_instance``
    wrappers."""
    if not ptr:
        return None
    kind = _take_scalar(lib, lib.ifcopenshell_ifcapi_value_kind, ctypes.c_int32, ptr)
    if kind == IFCSEL_VALUE_NONE:
        return None
    if kind == IFCSEL_VALUE_BOOL:
        value = _take_scalar(lib, lib.ifcopenshell_ifcapi_value_as_bool, ctypes.c_bool, ptr)
        return bool(value)
    if kind == IFCSEL_VALUE_INT:
        value = _take_scalar(lib, lib.ifcopenshell_ifcapi_value_as_int64, ctypes.c_int64, ptr)
        return int(value)
    if kind == IFCSEL_VALUE_DOUBLE:
        value = _take_scalar(lib, lib.ifcopenshell_ifcapi_value_as_double, ctypes.c_double, ptr)
        return float(value)
    if kind == IFCSEL_VALUE_STRING:
        return _take_string(lib, lib.ifcopenshell_ifcapi_value_as_string, ptr)
    if kind == IFCSEL_VALUE_INSTANCE:
        h = ctypes.POINTER(_generated_capi._HandleStruct)()
        if not lib.ifcopenshell_ifcapi_value_as_instance(ptr, ctypes.byref(h)) or not h:
            return None
        h_addr = ctypes.cast(h, ctypes.c_void_p).value
        if not h:
            return None
        # Local import avoids circular dep on entity_instance at module
        # load time.
        from ifcopenshell.entity_instance import entity_instance as _ei
        ifcopenshell_module = sys.modules["ifcopenshell"]
        file_ptr = ifcopenshell_module._instance_file_ptr(h_addr)
        return _ei(ifcopenshell_module._borrow_file_ptr(file_ptr, fallback=element.file), h_addr)
    if kind == IFCSEL_VALUE_LIST:
        n = _take_scalar(lib, lib.ifcopenshell_ifcapi_value_list_size, ctypes.c_size_t, ptr) or 0
        result = []
        for i in range(n):
            child = ctypes.POINTER(_generated_capi._HandleStruct)()
            if not lib.ifcopenshell_ifcapi_value_list_at(ptr, i, ctypes.byref(child)) or not child:
                result.append(None)
                continue
            try:
                result.append(value_to_python(lib, child, element))
            finally:
                lib.ifcopenshell_ifcapi_value_destroy(child)
        return result
    if kind == IFCSEL_VALUE_DICT:
        n = _take_scalar(lib, lib.ifcopenshell_ifcapi_value_dict_size, ctypes.c_size_t, ptr) or 0
        result = {}
        for i in range(n):
            key = _take_string(lib, lib.ifcopenshell_ifcapi_value_dict_key_at, ptr, i) or ""
            child = ctypes.POINTER(_generated_capi._HandleStruct)()
            if not lib.ifcopenshell_ifcapi_value_dict_value_at(ptr, i, ctypes.byref(child)) or not child:
                result[key] = None
                continue
            try:
                result[key] = value_to_python(lib, child, element)
            finally:
                lib.ifcopenshell_ifcapi_value_destroy(child)
        return result
    return None
