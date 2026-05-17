# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_geometry_add_boolean",
    "ifcopenshell_ifcapi_geometry_add_axis_representation",
    "ifcopenshell_ifcapi_geometry_add_footprint_representation",
    "ifcopenshell_ifcapi_geometry_add_mesh_representation",
    "ifcopenshell_ifcapi_geometry_add_shape_aspect",
    "ifcopenshell_ifcapi_geometry_add_slab_representation",
    "ifcopenshell_ifcapi_geometry_add_topology_representation",
    "ifcopenshell_ifcapi_geometry_add_wall_representation",
    "ifcopenshell_ifcapi_geometry_add_window_representation",
    "ifcopenshell_ifcapi_geometry_assign_representation",
    "ifcopenshell_ifcapi_geometry_clip_solid",
    "ifcopenshell_ifcapi_geometry_clip_solid_bounded",
    "ifcopenshell_ifcapi_geometry_connect_element",
    "ifcopenshell_ifcapi_geometry_connect_path",
    "ifcopenshell_ifcapi_geometry_connect_wall",
    "ifcopenshell_ifcapi_geometry_copy_representation",
    "ifcopenshell_ifcapi_geometry_create_2pt_wall",
    "ifcopenshell_ifcapi_geometry_disconnect_element",
    "ifcopenshell_ifcapi_geometry_disconnect_path",
    "ifcopenshell_ifcapi_geometry_edit_object_placement",
    "ifcopenshell_ifcapi_geometry_map_representation",
    "ifcopenshell_ifcapi_geometry_profile_extents",
    "ifcopenshell_ifcapi_geometry_remove_boolean",
    "ifcopenshell_ifcapi_geometry_remove_representation",
    "ifcopenshell_ifcapi_geometry_unassign_representation",
    "ifcopenshell_ifcapi_geometry_validate_type",
    "ifcopenshell_ifcapi_type_map_type_representations",
    "ifcopenshell_ifc_instance_destroy",
    "ifcopenshell_ifc_instance_list_destroy",
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


def int32_list(values):
    return _generated_capi.make_int32_list(values)


def int32_list_list(values):
    return _generated_capi.make_int32_list_list(values)


def double_list_list(values):
    return _generated_capi.make_double_list_list(values)


def bool_result(fn, *args) -> bool:
    lib = get_lib()
    return bool(_generated_capi.call_scalar_or_raise(lib, fn, ctypes.c_bool, f"{fn.__name__} failed", *args))


def double_list_list_list(values):
    return _generated_capi.make_double_list_list_list(values)


def int32_list_list_list_list(values):
    return _generated_capi.make_int32_list_list_list_list(values)


def owner_user_application(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return (
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )


def owner_context(file: ifcopenshell.file):
    user, application = owner_user_application(file)
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
