# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ctypes
import ifcopenshell
from ifcopenshell.entity_instance import entity_instance

from .reorder_nesting import reorder_nesting


def assign_object(file, related_objects=None, relating_object=None):
    """Assign objects as nested children of a host."""
    if not related_objects or relating_object is None:
        return None
    lib = ifcopenshell._get_lib()
    handles = [o._handle for o in related_objects]
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_nest_assign_object.restype = ctypes.c_void_p
    lib.ifcopenshell_nest_assign_object.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p,
    ]
    rel_h = lib.ifcopenshell_nest_assign_object(
        file._ptr, arr, len(handles), relating_object._handle
    )
    if not rel_h:
        return None
    return entity_instance(file, rel_h)


def unassign_object(file, related_objects=None):
    """Unassign objects from their nest."""
    if not related_objects:
        return
    lib = ifcopenshell._get_lib()
    handles = [o._handle for o in related_objects]
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_nest_unassign_object.restype = None
    lib.ifcopenshell_nest_unassign_object.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32,
    ]
    lib.ifcopenshell_nest_unassign_object(file._ptr, arr, len(handles))

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
