# SPDX-License-Identifier: LGPL-3.0-or-later
"""Type assignment API — thin wrappers around native C functions."""

import ctypes
from ... import _get_lib, entity_instance


def assign_type(file, related_objects=None, relating_type=None, should_map_representations=True):
    """Assign objects to a type via IfcRelDefinesByType.

    When ``should_map_representations`` is true (the default) this also
    propagates the relating type's ``IfcRepresentationMaps`` onto each
    related occurrence as ``IfcMappedItem``-based ``IfcShapeRepresentation``
    entries, and clears any redundant ``ObjectType`` / ``PredefinedType``
    on the occurrences (matching upstream Python behaviour).
    """
    lib = _get_lib()
    handles = [o._handle for o in (related_objects or []) if isinstance(o, entity_instance)]
    if not handles or relating_type is None:
        return None
    arr = (ctypes.c_void_p * len(handles))(*handles)
    rt_handle = relating_type._handle
    lib.ifcopenshell_type_assign_type_ex.restype = ctypes.c_void_p
    lib.ifcopenshell_type_assign_type_ex.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32,
        ctypes.c_void_p, ctypes.c_bool,
    ]
    rel_h = lib.ifcopenshell_type_assign_type_ex(
        file._ptr, arr, len(handles), rt_handle, bool(should_map_representations)
    )
    if not rel_h:
        return None
    return entity_instance(file, rel_h)


def unassign_type(file, related_objects=None):
    """Unassign objects from their type."""
    lib = _get_lib()
    handles = [o._handle for o in (related_objects or []) if isinstance(o, entity_instance)]
    if not handles:
        return
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_type_unassign_type.restype = None
    lib.ifcopenshell_type_unassign_type.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32,
    ]
    lib.ifcopenshell_type_unassign_type(file._ptr, arr, len(handles))

import importlib as _importlib

def __getattr__(name):
    try:
        module = _importlib.import_module(f".{name}", __name__)
    except ModuleNotFoundError:
        raise AttributeError(f"module {__name__!r} has no attribute {name!r}")
    func = getattr(module, name, None)
    if func is None:
        raise AttributeError(f"module {__name__!r} has no attribute {name!r}")
    globals()[name] = func
    return func
