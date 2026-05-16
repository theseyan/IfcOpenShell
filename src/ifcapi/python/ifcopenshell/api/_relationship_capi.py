# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes
from typing import Iterable

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_aggregate_assign_object",
    "ifcopenshell_ifcapi_aggregate_unassign_object",
    "ifcopenshell_ifcapi_spatial_assign_container",
    "ifcopenshell_ifcapi_spatial_unassign_container",
    "ifcopenshell_ifcapi_nest_assign_object",
    "ifcopenshell_ifcapi_nest_unassign_object",
    "ifcopenshell_ifcapi_group_add_group",
    "ifcopenshell_ifcapi_group_update_group_products",
    "ifcopenshell_ifcapi_group_assign_group",
    "ifcopenshell_ifcapi_group_unassign_group",
    "ifcopenshell_ifcapi_group_remove_group",
    "ifcopenshell_ifcapi_type_assign_type",
    "ifcopenshell_ifcapi_type_assign_type_ex",
    "ifcopenshell_ifcapi_type_unassign_type",
    "ifcopenshell_ifc_instance_destroy",
)


def get_lib() -> ctypes.CDLL:
    global _BOUND
    lib = _get_lib()
    if not _BOUND:
        _generated_capi.bind(lib, names=_BIND_NAMES)
        _BOUND = True
    return lib


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return _generated_instance_handle_ptr(entity._handle) if entity is not None else None


def file_handle(file: ifcopenshell.file):
    return _generated_instance_handle_ptr(file._ptr)


def instance_list(entities: Iterable[ifcopenshell.entity_instance]) -> _generated_capi.ifcopenshell_ifc_instance_list_t:
    handles = [instance_handle(entity) for entity in entities]
    items = (ctypes.POINTER(_generated_capi._HandleStruct) * len(handles))(*handles)
    result = _generated_capi.ifcopenshell_ifc_instance_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, handles)  # type: ignore[attr-defined]
    return result


def instance_list_ptr(value: _generated_capi.ifcopenshell_ifc_instance_list_t):
    return ctypes.byref(value)


def owner_user_application(file: ifcopenshell.file):
    return (
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )


def owner_context(file: ifcopenshell.file):
    user, application = owner_user_application(file)
    return None, user, application


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None


def call_handle(file: ifcopenshell.file, fn, *args):
    lib = get_lib()
    handle = _generated_capi.call_handle(
        lib,
        fn,
        *args,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    return wrap_handle(file, handle)


def call_status(fn, *args):
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), f"{fn.__name__} failed")
