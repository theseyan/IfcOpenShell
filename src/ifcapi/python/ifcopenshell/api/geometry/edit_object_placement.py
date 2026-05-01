# SPDX-License-Identifier: LGPL-3.0-or-later

"""Set the IfcLocalPlacement of a product via the native C API."""

from __future__ import annotations

import ctypes

import numpy as np

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False


def _bind() -> ctypes.CDLL:
    global _BOUND
    lib = ifcopenshell._get_lib()
    if _BOUND:
        return lib
    _generated_capi.bind(
        lib,
        names=("ifcopenshell_ifcapi_geometry_edit_object_placement", "ifcopenshell_ifc_instance_destroy"),
    )
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
        matrix_values = _generated_capi.make_double_list(_IDENTITY)
    else:
        arr = np.ascontiguousarray(np.asarray(matrix, dtype=np.float64).reshape(4, 4))
        matrix_values = _generated_capi.make_double_list(arr.reshape(16))

    file_handle = ctypes.cast(ctypes.c_void_p(file._ptr), ctypes.POINTER(_generated_capi._HandleStruct))
    handle = _generated_capi.call_handle(
        lib,
        lib.ifcopenshell_ifcapi_geometry_edit_object_placement,
        file_handle,
        _generated_instance_handle_ptr(product._handle),
        ctypes.byref(matrix_values),
        bool(is_si),
        bool(should_transform_children),
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    if not handle:
        return None
    new_placement = ifcopenshell.entity_instance(file, handle)
    ifcopenshell.api.owner.update_owner_history(file, element=product)
    return new_placement
