# SPDX-License-Identifier: LGPL-3.0-or-later

"""Thin wrapper for ifcopenshell.api.root.create_entity — delegates to native C++."""

import ctypes
from typing import Optional

import ifcopenshell


def create_entity(
    file: ifcopenshell.file,
    ifc_class: str = "IfcBuildingElementProxy",
    predefined_type: Optional[str] = None,
    name: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    """Create a new rooted IFC entity via the native backend."""
    lib = ifcopenshell._get_lib()
    lib.ifcopenshell_root_create_entity.restype = ctypes.c_void_p
    lib.ifcopenshell_root_create_entity.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_void_p,
    ]

    # Owner history is handled by the C++ backend when present.
    oh_handle = None
    try:
        histories = file.by_type("IfcOwnerHistory")
        if histories:
            oh_handle = histories[0]._handle
    except Exception:
        pass

    handle = lib.ifcopenshell_root_create_entity(
        file._ptr,
        ifcopenshell._enc(ifc_class),
        ifcopenshell._enc(predefined_type) if predefined_type else None,
        ifcopenshell._enc(name) if name else None,
        oh_handle,
    )
    if not handle:
        err = lib.ifcopenshell_last_error_message()
        msg = err.decode("utf-8") if err else "Unknown error"
        raise RuntimeError(f"Failed to create entity '{ifc_class}': {msg}")

    return ifcopenshell.entity_instance(file, handle)
