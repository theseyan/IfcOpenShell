# SPDX-License-Identifier: LGPL-3.0-or-later
"""Thin ctypes wrapper around the native ifcopenshell_geometry_copy_representation."""

from __future__ import annotations

import ctypes
from typing import Optional

import ifcopenshell
from ... import _get_lib, entity_instance


def copy_representation(
    file: ifcopenshell.file,
    source: ifcopenshell.entity_instance,
    target: ifcopenshell.entity_instance,
    context_identifier: str = "Body",
) -> Optional[ifcopenshell.entity_instance]:
    """Copy a named representation from ``source`` to ``target``.

    Looks up the representation on ``source`` whose
    ``RepresentationIdentifier`` equals ``context_identifier`` (default
    ``"Body"``), deep-copies its entity graph (sharing only
    ``IfcGeometricRepresentationContext``), removes any matching
    representation already on ``target``, and assigns the copy to
    ``target``. Returns the new representation, or ``None`` if ``source``
    has no matching representation.
    """
    if source is None or target is None:
        return None
    lib = _get_lib()
    lib.ifcopenshell_geometry_copy_representation.restype = ctypes.c_void_p
    lib.ifcopenshell_geometry_copy_representation.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p,
    ]
    new_h = lib.ifcopenshell_geometry_copy_representation(
        file._ptr,
        source._handle,
        target._handle,
        (context_identifier or "Body").encode("utf-8"),
    )
    if not new_h:
        return None
    return entity_instance(file, new_h)
