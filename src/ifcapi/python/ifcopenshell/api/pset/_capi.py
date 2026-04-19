# SPDX-License-Identifier: LGPL-3.0-or-later

"""ctypes bindings to the native ifcopenshell_api_pset_* C ABI."""

from __future__ import annotations

import ctypes
import datetime

import ifcopenshell
from ifcopenshell import _get_lib, _enc, _typed_value


_BOUND = False


def _bind() -> ctypes.CDLL:
    global _BOUND
    lib = _get_lib()
    if _BOUND:
        return lib

    # Builder
    lib.ifcopenshell_pset_props_new.restype = ctypes.c_void_p
    lib.ifcopenshell_pset_props_new.argtypes = []
    lib.ifcopenshell_pset_props_free.restype = None
    lib.ifcopenshell_pset_props_free.argtypes = [ctypes.c_void_p]

    lib.ifcopenshell_pset_props_set_null.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.ifcopenshell_pset_props_set_null.restype = None
    lib.ifcopenshell_pset_props_set_bool.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_bool]
    lib.ifcopenshell_pset_props_set_bool.restype = None
    lib.ifcopenshell_pset_props_set_int.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int64]
    lib.ifcopenshell_pset_props_set_int.restype = None
    lib.ifcopenshell_pset_props_set_double.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double]
    lib.ifcopenshell_pset_props_set_double.restype = None
    lib.ifcopenshell_pset_props_set_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    lib.ifcopenshell_pset_props_set_string.restype = None
    lib.ifcopenshell_pset_props_set_instance.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
    lib.ifcopenshell_pset_props_set_instance.restype = None

    lib.ifcopenshell_pset_props_set_typed_string.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p
    ]
    lib.ifcopenshell_pset_props_set_typed_string.restype = None
    lib.ifcopenshell_pset_props_set_typed_double.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p
    ]
    lib.ifcopenshell_pset_props_set_typed_double.restype = None
    lib.ifcopenshell_pset_props_set_typed_int.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int64, ctypes.c_char_p
    ]
    lib.ifcopenshell_pset_props_set_typed_int.restype = None
    lib.ifcopenshell_pset_props_set_typed_bool.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_bool, ctypes.c_char_p
    ]
    lib.ifcopenshell_pset_props_set_typed_bool.restype = None

    lib.ifcopenshell_pset_props_set_string_list.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.c_uint32
    ]
    lib.ifcopenshell_pset_props_set_string_list.restype = None
    lib.ifcopenshell_pset_props_set_double_list.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_uint32
    ]
    lib.ifcopenshell_pset_props_set_double_list.restype = None
    lib.ifcopenshell_pset_props_set_int_list.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int64), ctypes.c_uint32
    ]
    lib.ifcopenshell_pset_props_set_int_list.restype = None

    lib.ifcopenshell_pset_props_set_dict.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
    lib.ifcopenshell_pset_props_set_dict.restype = None

    lib.ifcopenshell_api_pset_add_pset.restype = ctypes.c_void_p
    lib.ifcopenshell_api_pset_add_pset.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p
    ]
    lib.ifcopenshell_api_pset_add_qto.restype = ctypes.c_void_p
    lib.ifcopenshell_api_pset_add_qto.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p
    ]
    lib.ifcopenshell_api_pset_edit_pset.restype = ctypes.c_bool
    lib.ifcopenshell_api_pset_edit_pset.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p,
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_bool,
    ]
    lib.ifcopenshell_api_pset_edit_qto.restype = ctypes.c_bool
    lib.ifcopenshell_api_pset_edit_qto.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p,
        ctypes.c_void_p, ctypes.c_void_p,
    ]

    _BOUND = True
    return lib


def _add_entry(lib, props, key, value):
    """Translate a single (key, value) into a builder call."""
    k = _enc(key)
    if value is None:
        lib.ifcopenshell_pset_props_set_null(props, k)
        return

    if isinstance(value, dict):
        inner = lib.ifcopenshell_pset_props_new()
        if not inner:
            raise RuntimeError("Failed to allocate nested property builder")
        discrim = value.get("Discrimination", "")
        lib.ifcopenshell_pset_props_set_string(inner, _enc("Discrimination"), _enc(str(discrim)))
        for k2, v2 in (value.get("HasQuantities") or {}).items():
            _add_entry(lib, inner, k2, v2)
        lib.ifcopenshell_pset_props_set_dict(props, k, inner)
        return

    # entity_instance — could be IfcProperty, IfcValue/typed value, or arbitrary entity.
    if isinstance(value, ifcopenshell.entity_instance):
        lib.ifcopenshell_pset_props_set_instance(props, k, value._handle)
        return

    if isinstance(value, _typed_value):
        ifc_type = value.is_a()
        py_val = value.wrappedValue
        if isinstance(py_val, bool):
            lib.ifcopenshell_pset_props_set_typed_bool(props, k, bool(py_val), _enc(ifc_type))
        elif isinstance(py_val, int):
            lib.ifcopenshell_pset_props_set_typed_int(props, k, int(py_val), _enc(ifc_type))
        elif isinstance(py_val, float):
            lib.ifcopenshell_pset_props_set_typed_double(props, k, float(py_val), _enc(ifc_type))
        else:
            lib.ifcopenshell_pset_props_set_typed_string(props, k, _enc(str(py_val)), _enc(ifc_type))
        return

    if isinstance(value, (list, tuple)):
        if not value:
            lib.ifcopenshell_pset_props_set_string_list(props, k, None, 0)
            return
        # Detect uniform element kind. Mixed -> coerce to strings.
        first = value[0]
        if all(isinstance(v, bool) or isinstance(v, int) and not isinstance(v, bool) for v in value):
            arr = (ctypes.c_int64 * len(value))(*[int(v) for v in value])
            lib.ifcopenshell_pset_props_set_int_list(props, k, arr, len(value))
        elif all(isinstance(v, (int, float)) and not isinstance(v, bool) for v in value):
            arr = (ctypes.c_double * len(value))(*[float(v) for v in value])
            lib.ifcopenshell_pset_props_set_double_list(props, k, arr, len(value))
        else:
            encoded = [_enc(str(v.wrappedValue) if isinstance(v, _typed_value) else str(v)) for v in value]
            arr = (ctypes.c_char_p * len(value))(*encoded)
            lib.ifcopenshell_pset_props_set_string_list(props, k, arr, len(value))
        return

    if isinstance(value, bool):
        lib.ifcopenshell_pset_props_set_bool(props, k, bool(value))
        return
    if isinstance(value, int):
        lib.ifcopenshell_pset_props_set_int(props, k, int(value))
        return
    if isinstance(value, float):
        lib.ifcopenshell_pset_props_set_double(props, k, float(value))
        return
    if isinstance(value, str):
        lib.ifcopenshell_pset_props_set_string(props, k, _enc(value))
        return
    if isinstance(value, datetime.datetime):
        lib.ifcopenshell_pset_props_set_typed_string(props, k, _enc(value.isoformat()), _enc("IfcDateTime"))
        return
    if isinstance(value, datetime.date):
        lib.ifcopenshell_pset_props_set_typed_string(props, k, _enc(value.isoformat()), _enc("IfcDate"))
        return

    # Fallback: stringify.
    lib.ifcopenshell_pset_props_set_string(props, k, _enc(str(value)))


def build_props(properties):
    """Allocate and populate an ifcopenshell_pset_props_t* from a dict."""
    lib = _bind()
    handle = lib.ifcopenshell_pset_props_new()
    if not handle:
        raise RuntimeError("Failed to allocate property builder")
    if properties:
        for key, value in properties.items():
            _add_entry(lib, handle, key, value)
    return handle


def free_props(handle):
    if not handle:
        return
    _bind().ifcopenshell_pset_props_free(handle)


def get_lib():
    return _bind()


def raise_last_error(default_msg):
    lib = _bind()
    err = lib.ifcopenshell_last_error_message()
    msg = err.decode("utf-8") if err else default_msg
    raise RuntimeError(msg)
