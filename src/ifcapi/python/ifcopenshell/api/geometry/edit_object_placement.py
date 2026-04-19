# SPDX-License-Identifier: LGPL-3.0-or-later

"""Set the IfcLocalPlacement of a product via the native C API."""

from __future__ import annotations

import ctypes

import numpy as np

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _get_lib


_BOUND = False


def _bind() -> ctypes.CDLL:
    global _BOUND
    lib = _get_lib()
    if _BOUND:
        return lib
    lib.ifcopenshell_api_geometry_edit_object_placement.restype = ctypes.c_void_p
    lib.ifcopenshell_api_geometry_edit_object_placement.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_bool,
        ctypes.c_bool,
    ]
    _BOUND = True
    return lib


_IDENTITY = (ctypes.c_double * 16)(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
)


def edit_object_placement(file, product=None, matrix=None, is_si=True, should_transform_children=False):
    if product is None or not hasattr(product, "ObjectPlacement"):
        return None

    lib = _bind()

    if matrix is None:
        matrix_ptr = _IDENTITY
    else:
        arr = np.ascontiguousarray(np.asarray(matrix, dtype=np.float64).reshape(4, 4))
        matrix_ptr = arr.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

    handle = lib.ifcopenshell_api_geometry_edit_object_placement(
        file._ptr,
        product._handle,
        matrix_ptr,
        bool(is_si),
        bool(should_transform_children),
    )
    if not handle:
        return None
    new_placement = ifcopenshell.entity_instance(file, handle)
    ifcopenshell.api.owner.update_owner_history(file, element=product)
    return new_placement
