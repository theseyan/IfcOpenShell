# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes
import datetime
import math

import isodate

import ifcopenshell
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.api.pset import _capi as pset_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_resource_edit_resource_time",
    "ifcopenshell_last_error_kind",
    "ifcopenshell_last_error_message",
)


def get_lib() -> ctypes.CDLL:
    global _BOUND
    lib = _get_lib()
    if not _BOUND:
        _generated_capi.bind(lib, names=_BIND_NAMES)
        _BOUND = True
    return lib


def file_handle(file: ifcopenshell.file):
    return _generated_instance_handle_ptr(file._ptr)


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return _generated_instance_handle_ptr(entity._handle) if entity is not None else None


def _set_temporal_entry(lib, props, key: str, value, parse_datetime_string: bool = False) -> bool:
    k = _generated_capi.encode_string(key)
    if parse_datetime_string and isinstance(value, str):
        try:
            value = datetime.datetime.fromisoformat(value)
        except ValueError:
            value = datetime.time.fromisoformat(value)
    if isinstance(value, datetime.datetime):
        offset = value.utcoffset()
        pset_capi._call(
            lib,
            lib.ifcopenshell_ifcapi_pset_props_set_datetime,
            props,
            k,
            value.year,
            value.month,
            value.day,
            value.hour,
            value.minute,
            value.second,
            value.microsecond,
            offset is not None,
            int(offset.total_seconds() // 60) if offset is not None else 0,
        )
        return True
    if isinstance(value, datetime.date):
        pset_capi._call(
            lib,
            lib.ifcopenshell_ifcapi_pset_props_set_datetime,
            props,
            k,
            value.year,
            value.month,
            value.day,
            0,
            0,
            0,
            0,
            False,
            0,
        )
        return True
    if isinstance(value, datetime.timedelta):
        _set_duration_entry(lib, props, k, value)
        return True
    if isinstance(value, isodate.duration.Duration):
        _set_duration_entry(lib, props, k, value)
        return True
    return False


def _split_timedelta(value: datetime.timedelta) -> tuple[bool, int, int, int, int, int]:
    total_microseconds = value.days * 86_400_000_000 + value.seconds * 1_000_000 + value.microseconds
    negative = total_microseconds < 0
    total_microseconds = abs(total_microseconds)
    days, rem = divmod(total_microseconds, 86_400_000_000)
    hours, rem = divmod(rem, 3_600_000_000)
    minutes, rem = divmod(rem, 60_000_000)
    seconds, microseconds = divmod(rem, 1_000_000)
    return negative, int(days), int(hours), int(minutes), int(seconds), int(microseconds)


def _duration_int(value) -> int:
    if value is None:
        return 0
    return int(math.trunc(value))


def _set_duration_entry(lib, props, key, value) -> None:
    years = months = 0
    delta = value
    if isinstance(value, isodate.duration.Duration):
        years = _duration_int(value.years)
        months = _duration_int(value.months)
        delta = value.tdelta
    negative, days, hours, minutes, seconds, microseconds = _split_timedelta(delta)
    pset_capi._call(
        lib,
        lib.ifcopenshell_ifcapi_pset_props_set_duration,
        props,
        key,
        negative,
        years,
        months,
        days,
        hours,
        minutes,
        seconds,
        microseconds,
    )


def build_resource_time_props(attributes):
    lib = pset_capi._bind()
    handle = pset_capi._new_props(lib)
    try:
        for key, value in (attributes or {}).items():
            if value and (("Start" in key) or ("Finish" in key) or key == "StatusTime") and _set_temporal_entry(
                lib, handle, key, value, parse_datetime_string=True
            ):
                continue
            if value and key in {"ScheduleWork", "ActualWork", "RemainingTime"} and _set_temporal_entry(
                lib, handle, key, value
            ):
                continue
            pset_capi._add_entry(lib, handle, key, value)
    except Exception:
        pset_capi.free_props(handle)
        raise
    return handle


def call_status(fn, *args) -> None:
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), f"{fn.__name__} failed")
    if _generated_capi.last_error_kind(lib) != _generated_capi.IFCOPENSHELL_ERROR_NONE:
        _generated_capi.raise_last_error(lib, f"{fn.__name__} failed")
