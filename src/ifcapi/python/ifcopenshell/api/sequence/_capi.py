# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
import ifcopenshell.api.owner

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return entity._handle if entity is not None else None


def string(value: str) -> str:
    return value


def owner_context(file: ifcopenshell.file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return None, user, application


def call_handle(file: ifcopenshell.file, fn_name: str, *args, nullable: bool = False):
    fn = getattr(_capi, fn_name)
    handle = fn(*args)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    if nullable:
        return None
    raise RuntimeError(f"{fn_name} failed")


def call_status(fn_name: str, *args) -> None:
    fn = getattr(_capi, fn_name)
    fn(*args)
