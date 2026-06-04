# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return entity._handle if entity is not None else None


def instance_list(entities):
    return [entity._handle for entity in entities]


def string(value: str) -> str:
    return value


def double_list(values):
    return list(values)


def int32_list(values):
    return list(values)


def int32_list_list(values):
    return [list(v) for v in values]


def double_list_list(values):
    return [list(v) for v in values]


def bool_result(fn_name: str, *args) -> bool:
    fn = getattr(_capi, fn_name)
    return bool(fn(*args))


def double_list_list_list(values):
    return [[list(v) for v in inner] for inner in values]


def int32_list_list_list_list(values):
    return [[[list(v) for v in inner2] for inner2 in inner1] for inner1 in values]


def owner_user_application(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return (
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )


def owner_context(file: ifcopenshell.file):
    user, application = owner_user_application(file)
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


def call_handle_list(file: ifcopenshell.file, fn_name: str, *args) -> list[ifcopenshell.entity_instance]:
    fn = getattr(_capi, fn_name)
    out = fn(*args)
    return [ifcopenshell.entity_instance(file, h) for h in out]
