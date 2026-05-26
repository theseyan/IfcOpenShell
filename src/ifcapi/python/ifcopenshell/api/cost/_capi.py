# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell.api.pset import _capi as pset_capi
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_cost_add_cost_item",
    "ifcopenshell_ifcapi_cost_add_cost_item_quantity",
    "ifcopenshell_ifcapi_cost_add_cost_schedule",
    "ifcopenshell_ifcapi_cost_add_cost_value",
    "ifcopenshell_ifcapi_cost_assign_cost_item_quantity",
    "ifcopenshell_ifcapi_cost_assign_cost_value",
    "ifcopenshell_ifcapi_cost_calculate_cost_item_resource_value",
    "ifcopenshell_ifcapi_cost_copy_cost_item",
    "ifcopenshell_ifcapi_cost_copy_cost_item_values",
    "ifcopenshell_ifcapi_cost_copy_cost_schedule",
    "ifcopenshell_ifcapi_cost_edit_cost_item",
    "ifcopenshell_ifcapi_cost_edit_cost_item_quantity",
    "ifcopenshell_ifcapi_cost_edit_cost_schedule",
    "ifcopenshell_ifcapi_cost_edit_cost_value",
    "ifcopenshell_ifcapi_cost_edit_cost_value_formula",
    "ifcopenshell_ifcapi_cost_remove_cost_item",
    "ifcopenshell_ifcapi_cost_remove_cost_item_quantity",
    "ifcopenshell_ifcapi_cost_remove_cost_schedule",
    "ifcopenshell_ifcapi_cost_remove_cost_value",
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


def instance_list(entities):
    handles = [instance_handle(entity) for entity in entities]
    items = (ctypes.POINTER(_generated_capi._HandleStruct) * len(handles))(*handles)
    result = _generated_capi.ifcopenshell_ifc_instance_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, handles)  # type: ignore[attr-defined]
    return result


def string(value: str) -> bytes:
    return _generated_capi.encode_string(value)


def owner_context(file: ifcopenshell.file):
    return None, ifcopenshell.api.owner.settings.get_user(file), ifcopenshell.api.owner.settings.get_application(file)


def call_handle(file: ifcopenshell.file, fn, *args):
    lib = get_lib()
    handle = _generated_capi.call_handle(lib, fn, *args, destroy=lib.ifcopenshell_ifc_instance_destroy)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    _generated_capi.raise_last_error(lib, f"{fn.__name__} failed")


def call_handle_list(file: ifcopenshell.file, fn, *args) -> list[ifcopenshell.entity_instance]:
    lib = get_lib()
    out = _generated_capi.ifcopenshell_ifc_instance_list_t()
    _generated_capi.status_or_raise(lib, fn(*args, ctypes.byref(out)), f"{fn.__name__} failed")
    handles = _generated_capi.move_handle_list(
        lib,
        out,
        lib.ifcopenshell_ifc_instance_list_destroy,
        ctypes.POINTER(_generated_capi.ifcopenshell_ifc_instance_t),
    )
    return [ifcopenshell.entity_instance(file, ctypes.cast(handle, ctypes.c_void_p).value) for handle in handles if handle]


def call_status(fn, *args) -> None:
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), f"{fn.__name__} failed")
    if _generated_capi.last_error_kind(lib) != _generated_capi.IFCOPENSHELL_ERROR_NONE:
        _generated_capi.raise_last_error(lib, f"{fn.__name__} failed")


def build_props(attributes):
    return pset_capi.build_props(attributes)


def free_props(handle):
    pset_capi.free_props(handle)
