# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_geometry_assign_representation",
    "ifcopenshell_ifcapi_geometry_copy_representation",
    "ifcopenshell_ifcapi_geometry_edit_object_placement",
    "ifcopenshell_ifcapi_geometry_map_representation",
    "ifcopenshell_ifcapi_geometry_profile_extents",
    "ifcopenshell_ifcapi_geometry_unassign_representation",
    "ifcopenshell_ifcapi_type_map_type_representations",
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
