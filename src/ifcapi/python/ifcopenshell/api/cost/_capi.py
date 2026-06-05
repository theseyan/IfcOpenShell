# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from ifcopenshell._capi_utils import (
    call_handle as _call_handle,
)
from ifcopenshell._capi_utils import (
    call_handle_list as _call_handle_list,
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
from ifcopenshell.api.pset import _capi as pset_capi

from ... import _ifcopenshell_capi as _capi


def string(value: str) -> str:
    return value


def build_props(attributes):
    return pset_capi.build_props(attributes)


def free_props(handle):
    pset_capi.free_props(handle)


def call_handle(file: ifcopenshell.file, fn_name: str, *args):
    return _call_handle(file, fn_name, *args, nullable=True)


def call_handle_list(file: ifcopenshell.file, fn_name: str, *args) -> list[ifcopenshell.entity_instance]:
    return _call_handle_list(file, fn_name, *args)


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)
