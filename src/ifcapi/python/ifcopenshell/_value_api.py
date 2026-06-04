# SPDX-License-Identifier: LGPL-3.0-or-later
# This file was generated with the assistance of an AI coding tool.
"""Low-level helpers for the ``ifcopenshell_value_t`` C ABI.

Extracted from ``ifcopenshell.util.selector`` so that modules deep in the
import chain (e.g. :mod:`ifcopenshell.entity_instance`) can reuse the
value marshalling helpers without triggering the larger selector module
import graph, which would cause a circular import.

Only depends on the native C API module and lightweight standard-library
helpers."""

from __future__ import annotations

import sys
from collections.abc import Iterable
from decimal import Decimal

from . import _ifcopenshell_capi as _capi

# ifcopenshell_value_kind_t constants (must match value.h)
IFCSEL_VALUE_NONE = 0
IFCSEL_VALUE_BOOL = 1
IFCSEL_VALUE_INT = 2
IFCSEL_VALUE_DOUBLE = 3
IFCSEL_VALUE_STRING = 4
IFCSEL_VALUE_INSTANCE = 5
IFCSEL_VALUE_LIST = 6
IFCSEL_VALUE_DICT = 7


def configure_value_lib(lib) -> None:
    """No-op; kept for API compatibility with callers."""


def python_to_value(lib, value):
    """Recursively convert a Python value to an owned ``ifcopenshell_value_t*``.

    The caller owns the returned pointer and must destroy it with
    ``value_destroy``.
    """
    if value is None:
        return _capi.value_new_none()
    if isinstance(value, bool):
        return _capi.value_new_bool(value)
    if isinstance(value, int):
        return _capi.value_new_int(value)
    if isinstance(value, float):
        return _capi.value_new_double(value)
    if isinstance(value, Decimal):
        return _capi.value_new_double(float(value))
    if isinstance(value, str):
        return _capi.value_new_string(value)
    from ifcopenshell.entity_instance import entity_instance

    if isinstance(value, entity_instance):
        return _capi.value_new_instance(value._handle)
    if isinstance(value, dict):
        raise TypeError(
            "ifcopenshell_value_t dict construction is not exposed by the generated C API."
        )
    if isinstance(value, Iterable):
        result = _capi.value_new_list()
        try:
            for item in value:
                child = python_to_value(lib, item)
                try:
                    _capi.value_list_append(result, child)
                finally:
                    _capi.value_destroy(child)
        except Exception:
            _capi.value_destroy(result)
            raise
        return result
    return _capi.value_new_string(str(value))


def value_to_python(lib, ptr, element):
    """Recursively convert an ``ifcopenshell_value_t*`` to a Python object.

    ``element`` is the originating entity used only to source the file
    handle when materialising nested INSTANCE values as ``entity_instance``
    wrappers."""
    if not ptr:
        return None
    kind = _capi.value_kind(ptr)
    if kind == IFCSEL_VALUE_NONE:
        return None
    if kind == IFCSEL_VALUE_BOOL:
        return bool(_capi.value_as_bool(ptr))
    if kind == IFCSEL_VALUE_INT:
        return int(_capi.value_as_int64(ptr))
    if kind == IFCSEL_VALUE_DOUBLE:
        return float(_capi.value_as_double(ptr))
    if kind == IFCSEL_VALUE_STRING:
        return _capi.value_as_string(ptr)
    if kind == IFCSEL_VALUE_INSTANCE:
        h = _capi.value_as_instance(ptr)
        if not h:
            return None
        from ifcopenshell.entity_instance import entity_instance as _ei

        ifcopenshell_module = sys.modules["ifcopenshell"]
        file_ptr = ifcopenshell_module._instance_file_ptr(h)
        return _ei(
            ifcopenshell_module._borrow_file_ptr(file_ptr, fallback=element.file), h
        )
    if kind == IFCSEL_VALUE_LIST:
        n = _capi.value_list_size(ptr) or 0
        result = []
        for i in range(n):
            child = _capi.value_list_at(ptr, i)
            if not child:
                result.append(None)
                continue
            try:
                result.append(value_to_python(lib, child, element))
            finally:
                _capi.value_destroy(child)
        return result
    if kind == IFCSEL_VALUE_DICT:
        n = _capi.value_dict_size(ptr) or 0
        result = {}
        for i in range(n):
            key = _capi.value_dict_key_at(ptr, i) or ""
            child = _capi.value_dict_value_at(ptr, i)
            if not child:
                result[key] = None
                continue
            try:
                result[key] = value_to_python(lib, child, element)
            finally:
                _capi.value_destroy(child)
        return result
    return None
