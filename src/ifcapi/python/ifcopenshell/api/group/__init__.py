# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ctypes
import ifcopenshell
import ifcopenshell.guid
import ifcopenshell.util.element
from ifcopenshell.entity_instance import entity_instance


def add_group(file, name="Unnamed", description=None):
    """Add a new IfcGroup."""
    return file.create_entity("IfcGroup", GlobalId=ifcopenshell.guid.new(), Name=name, Description=description)


def edit_group(file, group=None, attributes=None):
    """Edit the attributes of an IfcGroup."""
    if not attributes:
        return
    for name, value in attributes.items():
        setattr(group, name, value)


def assign_group(file, products=None, group=None):
    """Assign products to a group."""
    if not products or group is None:
        return None
    lib = ifcopenshell._get_lib()
    handles = [o._handle for o in products]
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_group_assign_group.restype = ctypes.c_void_p
    lib.ifcopenshell_group_assign_group.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p,
    ]
    rel_h = lib.ifcopenshell_group_assign_group(file._ptr, arr, len(handles), group._handle)
    if not rel_h:
        return None
    return entity_instance(file, rel_h)


def unassign_group(file, products=None, group=None):
    """Unassign products from a group."""
    if not products or group is None:
        return
    lib = ifcopenshell._get_lib()
    handles = [o._handle for o in products]
    arr = (ctypes.c_void_p * len(handles))(*handles)
    lib.ifcopenshell_group_unassign_group.restype = None
    lib.ifcopenshell_group_unassign_group.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p,
    ]
    lib.ifcopenshell_group_unassign_group(file._ptr, arr, len(handles), group._handle)


def remove_group(file, group=None):
    """Remove an IfcGroup and its relationships."""
    if group is None:
        return
    # Remove IfcRelAssignsToGroup relationships where this is the relating group
    is_grouped_by = getattr(group, "IsGroupedBy", ())
    for rel in list(is_grouped_by):
        if rel.RelatingGroup == group:
            history = getattr(rel, "OwnerHistory", None)
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    history = getattr(group, "OwnerHistory", None)
    file.remove(group)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)

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
