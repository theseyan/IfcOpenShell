# SPDX-License-Identifier: LGPL-3.0-or-later

"""Set the IfcLocalPlacement of a product via the native C API."""

from __future__ import annotations

import ctypes

import numpy as np

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _generated_capi
from ifcopenshell.api.geometry import _capi


_IDENTITY = (ctypes.c_double * 16)(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
)


def edit_object_placement(file, product=None, matrix=None, is_si=True, should_transform_children=False):
    if product is None or not hasattr(product, "ObjectPlacement"):
        return None

    lib = _capi.get_lib()

    if matrix is None:
        matrix_values = _generated_capi.make_double_list(_IDENTITY)
    else:
        arr = np.ascontiguousarray(np.asarray(matrix, dtype=np.float64).reshape(4, 4))
        matrix_values = _generated_capi.make_double_list(arr.reshape(16))

    handle = _generated_capi.call_handle(
        lib,
        lib.ifcopenshell_ifcapi_geometry_edit_object_placement,
        _capi.file_handle(file),
        _capi.instance_handle(product),
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
