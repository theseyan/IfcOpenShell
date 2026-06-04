# SPDX-License-Identifier: LGPL-3.0-or-later

"""Set the IfcLocalPlacement of a product via the native C API."""

from __future__ import annotations

import numpy as np

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _ifcopenshell_capi as _capi

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
        arr = np.ascontiguousarray(np.asarray(matrix, dtype=np.float64).reshape(4, 4))
        matrix_values = arr.reshape(16).tolist()

    handle = _capi.geometry_edit_object_placement(
        file._handle,
        product._handle,
        matrix_values,
        bool(is_si),
        bool(should_transform_children),
    )
    if not handle:
        return None
    new_placement = ifcopenshell.entity_instance(file, handle)
    ifcopenshell.api.owner.update_owner_history(file, element=product)
    return new_placement
