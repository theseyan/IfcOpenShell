# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_structural_add_structural_activity",
    "ifcopenshell_ifcapi_structural_add_structural_analysis_model",
    "ifcopenshell_ifcapi_structural_add_structural_load",
    "ifcopenshell_ifcapi_structural_add_structural_load_case",
    "ifcopenshell_ifcapi_structural_add_structural_load_group",
    "ifcopenshell_ifcapi_structural_add_structural_member_connection",
    "ifcopenshell_ifcapi_structural_add_structural_boundary_condition",
    "ifcopenshell_ifcapi_structural_assign_to_building",
    "ifcopenshell_ifcapi_structural_assign_product",
    "ifcopenshell_ifcapi_structural_assign_structural_analysis_model",
    "ifcopenshell_ifcapi_structural_edit_structural_connection_cs",
    "ifcopenshell_ifcapi_structural_edit_structural_item_axis",
    "ifcopenshell_ifcapi_structural_remove_structural_analysis_model",
    "ifcopenshell_ifcapi_structural_remove_structural_boundary_condition",
    "ifcopenshell_ifcapi_structural_remove_structural_connection_condition",
    "ifcopenshell_ifcapi_structural_remove_structural_load",
    "ifcopenshell_ifcapi_structural_remove_structural_load_case",
    "ifcopenshell_ifcapi_structural_remove_structural_load_group",
    "ifcopenshell_ifcapi_structural_unassign_structural_analysis_model",
    "ifcopenshell_ifc_instance_destroy",
    "ifcopenshell_last_error_kind",
    "ifcopenshell_last_error_message",
)


def get_lib() -> ctypes.CDLL:
    global _BOUND
    lib = ifcopenshell._get_lib()
    if not _BOUND:
        _generated_capi.bind(lib, names=_BIND_NAMES)
        _BOUND = True
    return lib


def file_handle(file: ifcopenshell.file):
    return ctypes.cast(ctypes.c_void_p(file._ptr), ctypes.POINTER(_generated_capi._HandleStruct))


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


def double_list(values):
    return _generated_capi.make_double_list(values)


def owner_history(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return ifcopenshell.api.owner.create_owner_history(file)


def owner_user_application(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return (
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )


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
