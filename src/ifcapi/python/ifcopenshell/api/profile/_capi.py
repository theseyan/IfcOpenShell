# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_profile_add_arbitrary_profile",
    "ifcopenshell_ifcapi_profile_add_arbitrary_profile_with_voids",
    "ifcopenshell_ifcapi_profile_add_parameterized_profile",
    "ifcopenshell_ifcapi_profile_copy_profile",
    "ifcopenshell_ifcapi_profile_edit_profile",
    "ifcopenshell_ifcapi_profile_remove_profile",
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


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None


def string(value: str) -> bytes:
    return _generated_capi.encode_string(value)


def double_list_list(values):
    return _generated_capi.make_double_list_list(values)


def double_list_list_list(values):
    return _generated_capi.make_double_list_list_list(values)


def call_status(fn, *args) -> None:
    lib = get_lib()
    _generated_capi.status_or_raise(lib, fn(*args), f"{fn.__name__} failed")
    if _generated_capi.last_error_kind(lib) != _generated_capi.IFCOPENSHELL_ERROR_NONE:
        _generated_capi.raise_last_error(lib, f"{fn.__name__} failed")
