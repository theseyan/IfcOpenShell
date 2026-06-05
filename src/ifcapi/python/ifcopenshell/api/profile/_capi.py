# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from ifcopenshell._capi_utils import (
    call_status as _call_status,
)
from ifcopenshell._capi_utils import (
    file_handle,
    wrap_handle,
)

from ... import _ifcopenshell_capi as _capi


def instance_handle(entity: ifcopenshell.entity_instance):
    return entity._handle


def string(value: str) -> str:
    return value


def double_list_list(values):
    return [list(v) for v in values]


def double_list_list_list(values):
    return [[list(v) for v in inner] for inner in values]


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)
