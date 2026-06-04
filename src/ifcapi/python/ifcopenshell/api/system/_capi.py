# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.api.owner.settings

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return entity._handle if entity is not None else None


def instance_list(entities):
    return [entity._handle for entity in entities]


def owner_context(file: ifcopenshell.file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return None, user, application


def create_owner_history(file: ifcopenshell.file):
    return ifcopenshell.api.owner.create_owner_history(file)


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None


def call_handle(file: ifcopenshell.file, fn_name: str, message: str, *args):
    fn = getattr(_capi, fn_name)
    handle = fn(*args)
    return wrap_handle(file, handle)


def call_nullable_handle(file: ifcopenshell.file, fn_name: str, *args):
    fn = getattr(_capi, fn_name)
    handle = fn(*args)
    return wrap_handle(file, handle)


def call_status(fn_name: str, message: str, *args) -> None:
    fn = getattr(_capi, fn_name)
    fn(*args)
