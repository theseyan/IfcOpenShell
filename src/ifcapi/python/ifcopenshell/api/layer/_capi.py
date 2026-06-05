# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from ifcopenshell._capi_utils import (
    file_handle,
    instance_list,
    wrap_handle,
)

from ... import _ifcopenshell_capi as _capi


def instance_handle(entity: ifcopenshell.entity_instance):
    return entity._handle


def logical_value(value: bool | str) -> int:
    if value is True:
        return _capi.IFCOPENSHELL_LOGICAL_TRUE
    if value is False:
        return _capi.IFCOPENSHELL_LOGICAL_FALSE
    if value == "UNKNOWN":
        return _capi.IFCOPENSHELL_LOGICAL_UNKNOWN
    raise ValueError('Logical value must be True, False, or "UNKNOWN"')
