# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell.api.pset import _capi as pset_capi

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return entity._handle if entity is not None else None


def instance_list(entities):
    return [entity._handle for entity in entities]


def string(value: str) -> str:
    return value


def owner_context(file: ifcopenshell.file):
    return None, ifcopenshell.api.owner.settings.get_user(file), ifcopenshell.api.owner.settings.get_application(file)


def call_handle(file: ifcopenshell.file, fn_name: str, *args):
    fn = getattr(_capi, fn_name)
    handle = fn(*args)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(f"{fn_name} failed")


def call_handle_list(file: ifcopenshell.file, fn_name: str, *args) -> list[ifcopenshell.entity_instance]:
    fn = getattr(_capi, fn_name)
    out = fn(*args)
    return [ifcopenshell.entity_instance(file, h) for h in out]


def call_status(fn_name: str, *args) -> None:
    fn = getattr(_capi, fn_name)
    fn(*args)


def build_props(attributes):
    return pset_capi.build_props(attributes)


def free_props(handle):
    pset_capi.free_props(handle)
