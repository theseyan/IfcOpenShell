# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
"""Thin ctypes wrapper around the native ifcopenshell_geometry_map_representation."""

from __future__ import annotations

import ctypes

import ifcopenshell
from ... import _get_lib, entity_instance


def map_representation(
    file: ifcopenshell.file, representation: ifcopenshell.entity_instance
) -> ifcopenshell.entity_instance:
    """Build an IfcShapeRepresentation of type "MappedRepresentation" that
    references ``representation`` via an IfcMappedItem. Reuses an existing
    IfcRepresentationMap inverse if present, otherwise creates one.
    """
    lib = _get_lib()
    lib.ifcopenshell_geometry_map_representation.restype = ctypes.c_void_p
    lib.ifcopenshell_geometry_map_representation.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    new_h = lib.ifcopenshell_geometry_map_representation(file._ptr, representation._handle)
    if not new_h:
        return None
    return entity_instance(file, new_h)
