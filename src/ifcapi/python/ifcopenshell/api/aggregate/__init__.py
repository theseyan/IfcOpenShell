# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
"""Aggregate assignment API — thin wrappers around native C functions."""

import ctypes
from ... import _get_lib, entity_instance


def assign_object(file, products=None, relating_object=None):
    """Assign products as parts of a whole via IfcRelAggregates.

    Returns the IfcRelAggregates entity_instance.
    """
    lib = _get_lib()
    handles = [p._handle for p in (products or []) if isinstance(p, entity_instance)]
    if not handles or relating_object is None:
        return None
    arr = (ctypes.c_void_p * len(handles))(*handles)
    ro_h = relating_object._handle
    lib.ifcopenshell_aggregate_assign_object.restype = ctypes.c_void_p
    lib.ifcopenshell_aggregate_assign_object.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p,
    ]
    rel_h = lib.ifcopenshell_aggregate_assign_object(file._ptr, arr, len(handles), ro_h)
    if not rel_h:
        return None
    return entity_instance(file, rel_h)


def unassign_object(file, products=None):
    """Unassign products from their aggregate."""
    lib = _get_lib()
    handles = [p._handle for p in (products or []) if isinstance(p, entity_instance)]
    if not handles:
        return
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_aggregate_unassign_object.restype = None
    lib.ifcopenshell_aggregate_unassign_object.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32,
    ]
    lib.ifcopenshell_aggregate_unassign_object(file._ptr, arr, len(handles))

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
