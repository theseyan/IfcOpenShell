# SPDX-License-Identifier: LGPL-3.0-or-later

"""Utility functions for classification data."""

import ctypes

import ifcopenshell
from ifcopenshell.entity_instance import entity_instance


_lib_configured = False


def _configure_lib(lib) -> None:
    global _lib_configured
    if _lib_configured:
        return
    lib.ifcopenshell_util_classification_get_references.restype = ctypes.POINTER(ctypes.c_void_p)
    lib.ifcopenshell_util_classification_get_references.argtypes = [
        ctypes.c_void_p, ctypes.c_bool, ctypes.POINTER(ctypes.c_uint32)
    ]
    lib.ifcopenshell_free_instance_array_only.restype = None
    lib.ifcopenshell_free_instance_array_only.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    _lib_configured = True


def get_references(element, should_inherit=True):
    if element is None:
        return set()
    lib = ifcopenshell._get_lib()
    _configure_lib(lib)
    count = ctypes.c_uint32(0)
    arr = lib.ifcopenshell_util_classification_get_references(
        element._handle, bool(should_inherit), ctypes.byref(count)
    )
    if not arr or count.value == 0:
        if arr:
            lib.ifcopenshell_free_instance_array_only(arr)
        return set()
    try:
        ifc_file = element.file
        return {entity_instance(ifc_file, arr[i]) for i in range(count.value)}
    finally:
        lib.ifcopenshell_free_instance_array_only(arr)


def get_classification(reference):
    if reference.is_a("IfcClassification"):
        return reference
    return get_classification(reference.ReferencedSource) if reference.ReferencedSource is not None else None
