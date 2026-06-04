# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return entity._handle if entity is not None else None


def owner_user_application(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return (
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )


def call_handle(file: ifcopenshell.file, fn_name: str, *args):
    fn = getattr(_capi, fn_name)
    handle = fn(*args)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(f"{fn_name} failed")


def call_status(fn_name: str, *args) -> None:
    fn = getattr(_capi, fn_name)
    fn(*args)
