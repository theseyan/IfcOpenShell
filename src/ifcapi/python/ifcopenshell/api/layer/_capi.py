# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes
from collections.abc import Iterable

import ifcopenshell
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_layer_add_layer",
    "ifcopenshell_ifcapi_layer_add_layer_with_style",
    "ifcopenshell_ifcapi_layer_assign_layer",
    "ifcopenshell_ifcapi_layer_remove_layer",
    "ifcopenshell_ifcapi_layer_unassign_layer",
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


def instance_handle(entity: ifcopenshell.entity_instance):
    return _generated_instance_handle_ptr(entity._handle)


def instance_list(entities: Iterable[ifcopenshell.entity_instance]) -> _generated_capi.ifcopenshell_ifc_instance_list_t:
    handles = [instance_handle(entity) for entity in entities]
    items = (ctypes.POINTER(_generated_capi._HandleStruct) * len(handles))(*handles)
    result = _generated_capi.ifcopenshell_ifc_instance_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, handles)  # type: ignore[attr-defined]
    return result


def logical_value(value: bool | str) -> int:
    if value is True:
        return _generated_capi.IFCOPENSHELL_LOGICAL_TRUE
    if value is False:
        return _generated_capi.IFCOPENSHELL_LOGICAL_FALSE
    if value == "UNKNOWN":
        return _generated_capi.IFCOPENSHELL_LOGICAL_UNKNOWN
    raise ValueError('Logical value must be True, False, or "UNKNOWN"')


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None
