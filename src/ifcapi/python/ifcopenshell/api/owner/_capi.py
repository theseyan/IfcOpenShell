# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ctypes

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _generated_capi, _get_lib
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False
_BIND_NAMES = (
    "ifcopenshell_ifcapi_owner_add_actor",
    "ifcopenshell_ifcapi_owner_add_address",
    "ifcopenshell_ifcapi_owner_add_application",
    "ifcopenshell_ifcapi_owner_add_organisation",
    "ifcopenshell_ifcapi_owner_add_person",
    "ifcopenshell_ifcapi_owner_add_person_and_organisation",
    "ifcopenshell_ifcapi_owner_add_role",
    "ifcopenshell_ifcapi_owner_assign_actor",
    "ifcopenshell_ifcapi_owner_remove_actor",
    "ifcopenshell_ifcapi_owner_remove_address",
    "ifcopenshell_ifcapi_owner_remove_application",
    "ifcopenshell_ifcapi_owner_remove_organisation",
    "ifcopenshell_ifcapi_owner_remove_person",
    "ifcopenshell_ifcapi_owner_remove_person_and_organisation",
    "ifcopenshell_ifcapi_owner_remove_role",
    "ifcopenshell_ifcapi_owner_unassign_actor",
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


def owner_context(file: ifcopenshell.file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return None, user, application


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
