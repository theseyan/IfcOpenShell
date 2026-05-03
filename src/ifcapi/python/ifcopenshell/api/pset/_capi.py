# SPDX-License-Identifier: LGPL-3.0-or-later

"""Helpers for the generated native pset C ABI."""

from __future__ import annotations

import ctypes
import datetime

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_pset_props_new",
    "ifcopenshell_ifcapi_pset_props_free",
    "ifcopenshell_ifcapi_pset_props_set_null",
    "ifcopenshell_ifcapi_pset_props_set_bool",
    "ifcopenshell_ifcapi_pset_props_set_int",
    "ifcopenshell_ifcapi_pset_props_set_double",
    "ifcopenshell_ifcapi_pset_props_set_string",
    "ifcopenshell_ifcapi_pset_props_set_instance",
    "ifcopenshell_ifcapi_pset_props_set_typed_string",
    "ifcopenshell_ifcapi_pset_props_set_typed_double",
    "ifcopenshell_ifcapi_pset_props_set_typed_int",
    "ifcopenshell_ifcapi_pset_props_set_typed_bool",
    "ifcopenshell_ifcapi_pset_props_set_string_list",
    "ifcopenshell_ifcapi_pset_props_set_double_list",
    "ifcopenshell_ifcapi_pset_props_set_int_list",
    "ifcopenshell_ifcapi_pset_props_set_dict",
    "ifcopenshell_ifcapi_pset_props_set_unit_for_last",
    "ifcopenshell_last_error_kind",
    "ifcopenshell_last_error_message",
)


def _bind() -> ctypes.CDLL:
    global _BOUND
    lib = _get_lib()
    if _BOUND:
        return lib

    _generated_capi.bind(lib, names=_BIND_NAMES)

    _BOUND = True
    return lib


def _call(lib, fn, *args):
    _generated_capi.status_or_raise(lib, fn(*args), f"{fn.__name__} failed")


def _new_props(lib):
    handle = _generated_capi.call_scalar(lib.ifcopenshell_ifcapi_pset_props_new, ctypes.c_void_p)
    if not handle:
        raise_last_error("Failed to allocate property builder")
    return handle


def _add_entry(lib, props, key, value):
    """Translate a single (key, value) into a builder call."""
    k = _generated_capi.encode_string(key)
    if value is None:
        _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_null, props, k)
        return

    if isinstance(value, dict):
        # ``{NominalValue, Unit}`` shape: a custom IfcUnit attached to a
        # property single value. Mirrors upstream's ``unpack_unit_value``.
        if "NominalValue" in value and "Unit" in value:
            _add_entry(lib, props, key, value["NominalValue"])
            unit = value["Unit"]
            unit_handle = _generated_instance_handle_ptr(unit._handle) if isinstance(unit, ifcopenshell.entity_instance) else None
            _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_unit_for_last, props, unit_handle)
            return
        inner = _new_props(lib)
        try:
            if "Discrimination" in value:
                _call(
                    lib,
                    lib.ifcopenshell_ifcapi_pset_props_set_string,
                    inner,
                    _generated_capi.encode_string("Discrimination"),
                    _generated_capi.encode_string(str(value["Discrimination"])),
                )
            if "HasQuantities" in value:
                for k2, v2 in (value["HasQuantities"] or {}).items():
                    _add_entry(lib, inner, k2, v2)
            _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_dict, props, k, inner)
        except Exception:
            free_props(inner)
            raise
        return

    # entity_instance — could be IfcProperty, IfcValue/typed value, or arbitrary entity.
    if isinstance(value, ifcopenshell.entity_instance):
        _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_instance, props, k, _generated_instance_handle_ptr(value._handle))
        return

    if isinstance(value, (list, tuple)):
        if not value:
            list_value = _generated_capi.make_string_list([])
            _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_string_list, props, k, ctypes.byref(list_value))
            return
        # Detect uniform element kind. Mixed -> coerce to strings.
        if all(isinstance(v, bool) or isinstance(v, int) and not isinstance(v, bool) for v in value):
            list_value = _generated_capi.make_int64_list([int(v) for v in value])
            _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_int_list, props, k, ctypes.byref(list_value))
        elif all(isinstance(v, (int, float)) and not isinstance(v, bool) for v in value):
            list_value = _generated_capi.make_double_list([float(v) for v in value])
            _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_double_list, props, k, ctypes.byref(list_value))
        else:
            list_value = _generated_capi.make_string_list(
                [
                    str(v.wrappedValue)
                    if isinstance(v, ifcopenshell.entity_instance) and v.id() == 0 and v._is_wrapped_value_instance()
                    else str(v)
                    for v in value
                ]
            )
            _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_string_list, props, k, ctypes.byref(list_value))
        return

    if isinstance(value, bool):
        _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_bool, props, k, bool(value))
        return
    if isinstance(value, int):
        _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_int, props, k, int(value))
        return
    if isinstance(value, float):
        _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_double, props, k, float(value))
        return
    if isinstance(value, str):
        _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_string, props, k, _generated_capi.encode_string(value))
        return
    if isinstance(value, datetime.datetime):
        _call(
            lib,
            lib.ifcopenshell_ifcapi_pset_props_set_typed_string,
            props,
            k,
            _generated_capi.encode_string(value.isoformat()),
            _generated_capi.encode_string("IfcDateTime"),
        )
        return
    if isinstance(value, datetime.date):
        _call(
            lib,
            lib.ifcopenshell_ifcapi_pset_props_set_typed_string,
            props,
            k,
            _generated_capi.encode_string(value.isoformat()),
            _generated_capi.encode_string("IfcDate"),
        )
        return

    # Fallback: stringify.
    _call(lib, lib.ifcopenshell_ifcapi_pset_props_set_string, props, k, _generated_capi.encode_string(str(value)))


def build_props(properties):
    """Allocate and populate an ifcopenshell_pset_props_t* from a dict."""
    lib = _bind()
    handle = _new_props(lib)
    if properties:
        for key, value in properties.items():
            _add_entry(lib, handle, key, value)
    return handle


def free_props(handle):
    if not handle:
        return
    lib = _bind()
    _call(lib, lib.ifcopenshell_ifcapi_pset_props_free, handle)


def get_lib():
    return _bind()


def instance_handle(entity):
    return _generated_instance_handle_ptr(entity._handle) if entity is not None else None


def owner_context(file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return None, user, application


def raise_last_error(default_msg):
    _generated_capi.raise_last_error(_bind(), default_msg)


def last_error_kind():
    return _generated_capi.last_error_kind(_bind())


def last_error_message(default_msg=""):
    lib = _bind()
    err = lib.ifcopenshell_last_error_message()
    return err.decode("utf-8") if err else default_msg
