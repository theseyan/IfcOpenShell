# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
"""Thin ctypes wrapper around the native ifcopenshell_type_map_type_representations."""

from __future__ import annotations

import ctypes

import ifcopenshell
from ... import _get_lib


def map_type_representations(
    file: ifcopenshell.file,
    related_object: ifcopenshell.entity_instance,
    relating_type: ifcopenshell.entity_instance,
) -> None:
    """Replace ``related_object``'s representations with mapped versions of
    every IfcRepresentationMap attached to ``relating_type``.
    """
    lib = _get_lib()
    lib.ifcopenshell_type_map_type_representations.restype = ctypes.c_bool
    lib.ifcopenshell_type_map_type_representations.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p,
    ]
    lib.ifcopenshell_type_map_type_representations(
        file._ptr, related_object._handle, relating_type._handle
    )
