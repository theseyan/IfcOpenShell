# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes
from collections.abc import Iterable

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.api.owner.settings
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_system_add_port",
    "ifcopenshell_ifcapi_system_add_system",
    "ifcopenshell_ifcapi_system_assign_flow_control",
    "ifcopenshell_ifcapi_system_assign_port",
    "ifcopenshell_ifcapi_system_assign_system",
    "ifcopenshell_ifcapi_system_connect_port",
    "ifcopenshell_ifcapi_system_disconnect_port",
    "ifcopenshell_ifcapi_system_remove_system",
    "ifcopenshell_ifcapi_system_unassign_flow_control",
    "ifcopenshell_ifcapi_system_unassign_port",
    "ifcopenshell_ifcapi_system_unassign_system",
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


def instance_list(entities: Iterable[ifcopenshell.entity_instance]) -> _generated_capi.ifcopenshell_ifc_instance_list_t:
    handles = [instance_handle(entity) for entity in entities]
    items = (ctypes.POINTER(_generated_capi._HandleStruct) * len(handles))(*handles)
    result = _generated_capi.ifcopenshell_ifc_instance_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, handles)  # type: ignore[attr-defined]
    return result


def owner_context(file: ifcopenshell.file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return None, user, application


def create_owner_history(file: ifcopenshell.file):
    return ifcopenshell.api.owner.create_owner_history(file)


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None


def call_handle(file: ifcopenshell.file, fn, message: str, *args):
    lib = get_lib()
    handle = _generated_capi.call_handle_or_raise(
        lib,
        fn,
        message,
        *args,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    return wrap_handle(file, handle)


def call_nullable_handle(file: ifcopenshell.file, fn, *args):
    lib = get_lib()
    handle = _generated_capi.call_handle_or_raise(
        lib,
        fn,
        f"{fn.__name__} failed",
        *args,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    return wrap_handle(file, handle)


def call_status(fn, message: str, *args) -> None:
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), message)
