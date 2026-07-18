# SPDX-License-Identifier: LGPL-3.0-or-later

"""Set the IfcLocalPlacement of a product via the native C API."""

from __future__ import annotations

import numpy as np

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell.api.geometry import _capi

_IDENTITY = [
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
]


def edit_object_placement(file, product=None, matrix=None, is_si=True, should_transform_children=False):
    if product is None or not hasattr(product, "ObjectPlacement"):
        return None

    if matrix is None:
        matrix_values = _IDENTITY
    else:
        arr = np.asarray(matrix, dtype=np.float64)
        if arr.shape != (4, 4):
            raise ValueError(f"Expected matrix with shape (4, 4), got {arr.shape}")
        arr = np.ascontiguousarray(arr)
        matrix_values = arr.reshape(16).tolist()

    new_placement = _capi.call_handle(
        file,
        "geometry_edit_object_placement",
        _capi.file_handle(file),
        {
            "product": _capi.instance_handle(product),
            "matrix": matrix_values,
            "is_si": bool(is_si),
            "should_transform_children": bool(should_transform_children),
        },
        nullable=True,
    )
    ifcopenshell.api.owner.update_owner_history(file, element=product)
    return new_placement
