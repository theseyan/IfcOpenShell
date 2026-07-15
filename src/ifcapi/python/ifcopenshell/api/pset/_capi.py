# SPDX-License-Identifier: LGPL-3.0-or-later

"""Helpers for the generated native pset C ABI."""

from __future__ import annotations

import datetime

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell._capi_utils import (
    file_handle,
    instance_handle,
    instance_list,
    owner_context,
    raise_last_error,
)

from ... import _ifcopenshell_capi as _capi


def _call(fn_name: str, *args):
    fn = getattr(_capi, fn_name)
    fn(*args)


def _new_props():
    return _capi.pset_props_new()


def _add_entry(props, key, value):
    """Translate a single (key, value) into a builder call."""
    if value is None:
        _call("ifcopenshell_pset_props_set_null", props, key)
        return

    if isinstance(value, dict):
        # ``{NominalValue, Unit}`` shape: a custom IfcUnit attached to a
        # property single value. Mirrors upstream's ``unpack_unit_value``.
        if "NominalValue" in value and "Unit" in value:
            _add_entry(props, key, value["NominalValue"])
            unit = value["Unit"]
            unit_handle = (
                unit._handle if isinstance(unit, ifcopenshell.entity_instance) else None
            )
            _call("ifcopenshell_pset_props_set_unit_for_last", props, unit_handle)
            return
        inner = _new_props()
        try:
            if "Discrimination" in value or "HasQuantities" in value:
                if "Discrimination" in value:
                    _call(
                        "ifcopenshell_pset_props_set_string",
                        inner,
                        "Discrimination",
                        str(value["Discrimination"]),
                    )
                if "HasQuantities" in value:
                    for k2, v2 in (value["HasQuantities"] or {}).items():
                        _add_entry(inner, k2, v2)
            else:
                for k2, v2 in value.items():
                    _add_entry(inner, k2, v2)
            _call("ifcopenshell_pset_props_set_dict", props, key, inner)
        except Exception:
            free_props(inner)
            raise
        return

    # entity_instance -- could be IfcProperty, IfcValue/typed value, or arbitrary entity.
    if isinstance(value, ifcopenshell.entity_instance):
        _call("ifcopenshell_pset_props_set_instance", props, key, value._handle)
        return

    if isinstance(value, (list, tuple)):
        if not value:
            _call("ifcopenshell_pset_props_set_string_list", props, key, [])
            return
        if all(isinstance(v, ifcopenshell.entity_instance) for v in value):
            _call(
                "ifcopenshell_pset_props_set_instance_list",
                props,
                key,
                [v._handle for v in value],
            )
            return
        # Detect uniform element kind. Mixed -> coerce to strings.
        if all(isinstance(v, bool) for v in value):
            _call("ifcopenshell_pset_props_set_bool_list", props, key, list(value))
        elif all(isinstance(v, int) and not isinstance(v, bool) for v in value):
            _call(
                "ifcopenshell_pset_props_set_int_list",
                props,
                key,
                [int(v) for v in value],
            )
        elif all(
            isinstance(v, (int, float)) and not isinstance(v, bool) for v in value
        ):
            _call(
                "ifcopenshell_pset_props_set_double_list",
                props,
                key,
                [float(v) for v in value],
            )
        else:
            _call(
                "ifcopenshell_pset_props_set_string_list",
                props,
                key,
                [
                    str(v.wrappedValue)
                    if isinstance(v, ifcopenshell.entity_instance)
                    and v.id() == 0
                    and v._is_wrapped_value_instance()
                    else str(v)
                    for v in value
                ],
            )
        return

    if isinstance(value, bool):
        _call("ifcopenshell_pset_props_set_bool", props, key, bool(value))
        return
    if isinstance(value, int):
        _call("ifcopenshell_pset_props_set_int", props, key, int(value))
        return
    if isinstance(value, float):
        _call("ifcopenshell_pset_props_set_double", props, key, float(value))
        return
    if isinstance(value, str):
        _call("ifcopenshell_pset_props_set_string", props, key, value)
        return
    if isinstance(value, datetime.datetime):
        _call(
            "ifcopenshell_pset_props_set_typed_string",
            props,
            key,
            value.isoformat(),
            "IfcDateTime",
        )
        return
    if isinstance(value, datetime.date):
        _call(
            "ifcopenshell_pset_props_set_typed_string",
            props,
            key,
            value.isoformat(),
            "IfcDate",
        )
        return

    # Fallback: stringify.
    _call("ifcopenshell_pset_props_set_string", props, key, str(value))


def build_props(properties):
    """Allocate and populate an ifcopenshell_pset_props_t* from a dict."""
    handle = _new_props()
    if properties:
        for key, value in properties.items():
            _add_entry(handle, key, value)
    return handle


def free_props(handle):
    if not handle:
        return
    _call("ifcopenshell_pset_props_free", handle)


def last_error_kind():
    return _capi.last_error_kind()


def last_error_message(default_msg=""):
    err = _capi.last_error_message()
    return err if err else default_msg
