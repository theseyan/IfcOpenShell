# SPDX-License-Identifier: LGPL-3.0-or-later
"""Thin ctypes wrapper around the native ifcopenshell_geometry_assign_representation."""

from __future__ import annotations

import ctypes

import ifcopenshell
from ... import _get_lib, entity_instance


def assign_representation(
    file: ifcopenshell.file,
    product: ifcopenshell.entity_instance,
    representation: ifcopenshell.entity_instance,
) -> ifcopenshell.entity_instance:
    """Assign ``representation`` to ``product`` (or its type, if it has
    geometric RepresentationMaps). Returns the (possibly re-routed)
    product on success.
    """
    lib = _get_lib()
    lib.ifcopenshell_geometry_assign_representation.restype = ctypes.c_void_p
    lib.ifcopenshell_geometry_assign_representation.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p,
    ]
    new_h = lib.ifcopenshell_geometry_assign_representation(
        file._ptr, product._handle, representation._handle
    )
    if not new_h:
        return None
    return entity_instance(file, new_h)
