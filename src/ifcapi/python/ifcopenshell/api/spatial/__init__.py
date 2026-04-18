# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
"""Spatial containment API — thin wrappers around native C functions."""

import ctypes
from ... import _get_lib, entity_instance


def assign_container(file, products=None, relating_structure=None):
    """Assign products to a spatial container via IfcRelContainedInSpatialStructure.

    Returns the IfcRelContainedInSpatialStructure entity_instance.
    """
    lib = _get_lib()
    handles = [p._handle for p in (products or []) if isinstance(p, entity_instance)]
    if not handles or relating_structure is None:
        return None
    arr = (ctypes.c_void_p * len(handles))(*handles)
    rs_h = relating_structure._handle
    lib.ifcopenshell_spatial_assign_container.restype = ctypes.c_void_p
    lib.ifcopenshell_spatial_assign_container.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p,
    ]
    rel_h = lib.ifcopenshell_spatial_assign_container(file._ptr, arr, len(handles), rs_h)
    if not rel_h:
        return None
    return entity_instance(file, rel_h)


def unassign_container(file, products=None):
    """Unassign products from their spatial container."""
    lib = _get_lib()
    handles = [p._handle for p in (products or []) if isinstance(p, entity_instance)]
    if not handles:
        return
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_spatial_unassign_container.restype = None
    lib.ifcopenshell_spatial_unassign_container.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32,
    ]
    lib.ifcopenshell_spatial_unassign_container(file._ptr, arr, len(handles))

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
