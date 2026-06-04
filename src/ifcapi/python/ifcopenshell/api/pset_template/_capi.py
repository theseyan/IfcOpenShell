# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell

from ... import _ifcopenshell_capi as _capi


def file_handle(file: ifcopenshell.file):
    return file._handle


def instance_handle(entity: ifcopenshell.entity_instance | None):
    return entity._handle if entity is not None else None


def optional_string(value: str | None) -> str | None:
    return value


def wrap_handle(file: ifcopenshell.file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None
