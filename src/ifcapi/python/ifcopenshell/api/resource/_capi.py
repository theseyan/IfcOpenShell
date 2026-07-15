# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import datetime
import math

import isodate

import ifcopenshell
from ifcopenshell._capi_utils import (
    call_handle as _call_handle,
)
from ifcopenshell._capi_utils import (
    call_status as _call_status,
)
from ifcopenshell._capi_utils import (
    file_handle,
    instance_handle,
)
from ifcopenshell.api.pset import _capi as pset_capi

from ... import _ifcopenshell_capi as _capi


def invalidate(instance: ifcopenshell.entity_instance) -> None:
    _capi.instance_destroy(instance._handle)
    instance._handle = None


def _set_temporal_entry(
    props, key: str, value, parse_datetime_string: bool = False
) -> bool:
    if parse_datetime_string and isinstance(value, str):
        try:
            value = datetime.datetime.fromisoformat(value)
        except ValueError:
            value = datetime.time.fromisoformat(value)
    if isinstance(value, datetime.datetime):
        offset = value.utcoffset()
        pset_capi._call(
            "ifcopenshell_pset_props_set_datetime",
            props,
            key,
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
            "ifcopenshell_pset_props_set_datetime",
            props,
            key,
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
        _set_duration_entry(props, key, value)
        return True
    if isinstance(value, isodate.duration.Duration):
        _set_duration_entry(props, key, value)
        return True
    return False


def _split_timedelta(value: datetime.timedelta) -> tuple[bool, int, int, int, int, int]:
    total_microseconds = (
        value.days * 86_400_000_000 + value.seconds * 1_000_000 + value.microseconds
    )
    negative = total_microseconds < 0
    total_microseconds = abs(total_microseconds)
    days, rem = divmod(total_microseconds, 86_400_000_000)
    hours, rem = divmod(rem, 3_600_000_000)
    minutes, rem = divmod(rem, 60_000_000)
    seconds, microseconds = divmod(rem, 1_000_000)
    return (
        negative,
        int(days),
        int(hours),
        int(minutes),
        int(seconds),
        int(microseconds),
    )


def _duration_int(value) -> int:
    if value is None:
        return 0
    return int(math.trunc(value))


def _set_duration_entry(props, key, value) -> None:
    years = months = 0
    delta = value
    if isinstance(value, isodate.duration.Duration):
        years = _duration_int(value.years)
        months = _duration_int(value.months)
        delta = value.tdelta
    negative, days, hours, minutes, seconds, microseconds = _split_timedelta(delta)
    pset_capi._call(
        "ifcopenshell_pset_props_set_duration",
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
    handle = pset_capi._new_props()
    try:
        for key, value in (attributes or {}).items():
            if (
                value
                and (("Start" in key) or ("Finish" in key) or key == "StatusTime")
                and _set_temporal_entry(handle, key, value, parse_datetime_string=True)
            ):
                continue
            if (
                value
                and key in {"ScheduleWork", "ActualWork", "RemainingTime"}
                and _set_temporal_entry(handle, key, value)
            ):
                continue
            pset_capi._add_entry(handle, key, value)
    except Exception:
        pset_capi.free_props(handle)
        raise
    return handle


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)


def call_handle(file, fn_name: str, *args):
    return _call_handle(file, fn_name, *args, nullable=False)


def owner_options(file, create_history: bool = False):
    import ifcopenshell.api.owner
    import ifcopenshell.api.owner.settings

    history = (
        ifcopenshell.api.owner.create_owner_history(file) if create_history else None
    )
    values = {
        "owner_history": history,
        "user": ifcopenshell.api.owner.settings.get_user(file),
        "application": ifcopenshell.api.owner.settings.get_application(file),
    }
    return (
        {
            key: instance_handle(value)
            for key, value in values.items()
            if value is not None
        },
        tuple(values.values()),
    )
