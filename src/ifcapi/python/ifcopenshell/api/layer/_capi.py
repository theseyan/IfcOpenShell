# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance):
    return entity._handle


def instance_list(entities):
    return [entity._handle for entity in entities]


def logical_value(value: bool | str) -> int:
    if value is True:
        return _capi.IFCOPENSHELL_LOGICAL_TRUE
    if value is False:
        return _capi.IFCOPENSHELL_LOGICAL_FALSE
    if value == "UNKNOWN":
        return _capi.IFCOPENSHELL_LOGICAL_UNKNOWN
    raise ValueError('Logical value must be True, False, or "UNKNOWN"')


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None
