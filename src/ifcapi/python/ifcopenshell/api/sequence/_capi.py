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
)

from ... import _ifcopenshell_capi as _capi


def string(value: str) -> str:
    return value


def call_handle(file: ifcopenshell.file, fn_name: str, *args, nullable: bool = False):
    return _call_handle(file, fn_name, *args, nullable=True)


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)
