# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_sequence_add_date_time",
    "ifcopenshell_ifcapi_sequence_add_task",
    "ifcopenshell_ifcapi_sequence_add_task_time",
    "ifcopenshell_ifcapi_sequence_add_time_period",
    "ifcopenshell_ifcapi_sequence_add_work_calendar",
    "ifcopenshell_ifcapi_sequence_add_work_plan",
    "ifcopenshell_ifcapi_sequence_add_work_schedule",
    "ifcopenshell_ifcapi_sequence_add_work_time",
    "ifcopenshell_ifcapi_sequence_assign_lag_time",
    "ifcopenshell_ifcapi_sequence_assign_process",
    "ifcopenshell_ifcapi_sequence_assign_product",
    "ifcopenshell_ifcapi_sequence_assign_recurrence_pattern",
    "ifcopenshell_ifcapi_sequence_assign_sequence",
    "ifcopenshell_ifcapi_sequence_assign_work_plan",
    "ifcopenshell_ifcapi_sequence_calculate_task_duration",
    "ifcopenshell_ifcapi_sequence_cascade_schedule",
    "ifcopenshell_ifcapi_sequence_copy_work_schedule",
    "ifcopenshell_ifcapi_sequence_create_baseline",
    "ifcopenshell_ifcapi_sequence_duplicate_task",
    "ifcopenshell_ifcapi_sequence_edit_task_time",
    "ifcopenshell_ifcapi_sequence_recalculate_schedule",
    "ifcopenshell_ifcapi_sequence_edit_lag_time",
    "ifcopenshell_ifcapi_sequence_edit_recurrence_pattern",
    "ifcopenshell_ifcapi_sequence_edit_sequence",
    "ifcopenshell_ifcapi_sequence_edit_task",
    "ifcopenshell_ifcapi_sequence_edit_work_calendar",
    "ifcopenshell_ifcapi_sequence_edit_work_plan",
    "ifcopenshell_ifcapi_sequence_edit_work_schedule",
    "ifcopenshell_ifcapi_sequence_edit_work_time",
    "ifcopenshell_ifcapi_sequence_remove_task",
    "ifcopenshell_ifcapi_sequence_remove_time_period",
    "ifcopenshell_ifcapi_sequence_remove_work_calendar",
    "ifcopenshell_ifcapi_sequence_remove_work_plan",
    "ifcopenshell_ifcapi_sequence_remove_work_schedule",
    "ifcopenshell_ifcapi_sequence_remove_work_time",
    "ifcopenshell_ifcapi_sequence_unassign_lag_time",
    "ifcopenshell_ifcapi_sequence_unassign_process",
    "ifcopenshell_ifcapi_sequence_unassign_product",
    "ifcopenshell_ifcapi_sequence_unassign_recurrence_pattern",
    "ifcopenshell_ifcapi_sequence_unassign_sequence",
    "ifcopenshell_ifc_instance_list_destroy",
    "ifcopenshell_ifc_instance_destroy",
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


def string(value: str) -> bytes:
    return _generated_capi.encode_string(value)


def owner_context(file: ifcopenshell.file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return None, user, application


def call_handle(file: ifcopenshell.file, fn, *args, nullable: bool = False):
    lib = get_lib()
    handle = _generated_capi.call_handle(
        lib,
        fn,
        *args,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    if nullable and _generated_capi.last_error_kind(lib) == _generated_capi.IFCOPENSHELL_ERROR_NONE:
        return None
    _generated_capi.raise_last_error(lib, f"{fn.__name__} failed")


def call_status(fn, *args) -> None:
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), f"{fn.__name__} failed")
    if _generated_capi.last_error_kind(lib) != _generated_capi.IFCOPENSHELL_ERROR_NONE:
        _generated_capi.raise_last_error(lib, f"{fn.__name__} failed")
