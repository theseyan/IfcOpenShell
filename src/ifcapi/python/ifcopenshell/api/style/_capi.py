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
    "ifcopenshell_ifcapi_style_add_style",
    "ifcopenshell_ifcapi_style_assign_item_style",
    "ifcopenshell_ifcapi_style_assign_material_style",
    "ifcopenshell_ifcapi_style_assign_representation_styles",
    "ifcopenshell_ifcapi_style_edit_surface_style",
    "ifcopenshell_ifcapi_style_remove_style",
    "ifcopenshell_ifcapi_style_remove_styled_representation",
    "ifcopenshell_ifcapi_style_remove_surface_style",
    "ifcopenshell_ifcapi_style_unassign_material_style",
    "ifcopenshell_ifcapi_style_unassign_representation_styles",
    "ifcopenshell_ifc_instance_destroy",
    "ifcopenshell_ifc_instance_list_destroy",
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


def call_status(fn, message: str, *args) -> None:
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), message)


def call_handle_list(file: ifcopenshell.file, fn, message: str, *args) -> list[ifcopenshell.entity_instance]:
    lib = get_lib()
    out = ctypes.POINTER(_generated_capi._HandleStruct)()
    _generated_capi.status_or_raise(lib, fn(*args, ctypes.byref(out)), message)
    return ifcopenshell._take_instance_list(file, out)
