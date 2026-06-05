# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from ifcopenshell._capi_utils import (
    call_handle as _call_handle,
)
from ifcopenshell._capi_utils import (
    call_status as _call_status,
)
from ifcopenshell._capi_utils import (
    file_handle,
    instance_handle,
    instance_list,
    owner_context,
    wrap_handle,
)

from ... import _ifcopenshell_capi as _capi


def string(value: str) -> str:
    return value


def double_list(values):
    return list(values)


def owner_history(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return ifcopenshell.api.owner.create_owner_history(file)


def owner_user_application(file: ifcopenshell.file):
    import ifcopenshell.api.owner

    return (
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )


def call_handle(file: ifcopenshell.file, fn_name: str, *args, nullable: bool = False):
    return _call_handle(file, fn_name, *args, nullable=nullable)


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)
