# SPDX-License-Identifier: LGPL-3.0-or-later

"""Placement utilities backed by the native ifcapi C library.

Pure math helpers (``a2p``, ``rotation``) call into C. Entity-aware helpers
(``get_axis2placement``, ``get_local_placement``, etc.) forward to the native
library which walks the IFC graph directly. Results are returned as row-major
4x4 ``numpy.ndarray[float64]`` to match upstream's public contract.
"""

from __future__ import annotations

from collections.abc import Iterable
from typing import Literal, Optional

import numpy as np
import numpy.typing as npt

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi

MatrixType = npt.NDArray[np.float64]
"""`npt.NDArray[np.float64]`"""


def _matrix_from_values(values: Optional[Iterable[float]], *, fallback: str) -> MatrixType:
    if values is None:
        raise RuntimeError(fallback)
    values = list(values)
    if len(values) != 16:
        raise RuntimeError(fallback)
    return np.array(values, dtype=np.float64).reshape((4, 4))


def a2p(o: Iterable[float], z: Iterable[float], x: Iterable[float]) -> MatrixType:
    """Build a 4x4 transformation matrix from origin, Z axis, and X axis."""
    values = _capi.placement_a2p(list(o), list(z), list(x))
    return _matrix_from_values(values, fallback="a2p failed to compute a 4x4 placement matrix")


def get_axis2placement(placement: ifcopenshell.entity_instance) -> MatrixType:
    """Parse an IfcAxis2Placement (2D, 3D, Linear, or 1D) into a 4x4 matrix."""
    matrix_values = _capi.placement_get_axis2placement(placement._handle)
    if matrix_values and len(matrix_values) == 16:
        return _matrix_from_values(matrix_values, fallback="Failed to compute axis placement matrix")
    # Fallback for IfcAxis2PlacementLinear with IfcPointByDistanceExpression.
    if placement.is_a("IfcAxis2PlacementLinear"):
        import ifcopenshell.geom
        import ifcopenshell.ifcopenshell_wrapper as ifcopenshell_wrapper
        import ifcopenshell.util.unit

        location = placement.Location
        basis_curve = getattr(location, "BasisCurve", None)
        distance = getattr(location, "DistanceAlong", None)
        if basis_curve is None or distance is None:
            raise RuntimeError("IfcAxis2PlacementLinear is missing a basis curve or distance expression")

        unit_scale = ifcopenshell.util.unit.calculate_unit_scale(placement.file)
        distance_along = getattr(distance, "wrappedValue", distance)
        offset_lateral = getattr(getattr(location, "OffsetLateral", 0.0), "wrappedValue", location.OffsetLateral or 0.0)

        settings = ifcopenshell.geom.settings()
        settings.set("convert-back-units", True)
        fn = ifcopenshell_wrapper.map_shape(settings, basis_curve.wrapped_data)
        evaluator = ifcopenshell_wrapper.function_item_evaluator(settings, fn)
        matrix = np.array(evaluator.evaluate(float(distance_along) * unit_scale), dtype=np.float64)
        matrix[0:3, 3] = matrix[0:3, 3] / unit_scale + matrix[0:3, 1] * float(offset_lateral)
        return matrix
    raise RuntimeError(f"Failed to compute axis placement matrix for {placement.is_a()}")


def get_local_placement(placement: Optional[ifcopenshell.entity_instance] = None) -> MatrixType:
    """Parse an IfcLocalPlacement into a 4x4 transformation matrix."""
    if placement is None:
        return np.eye(4)
    if (rel_to := placement.PlacementRelTo) is None:
        parent = np.eye(4)
    else:
        parent = get_local_placement(rel_to)
    return np.dot(parent, get_axis2placement(placement.RelativePlacement))


def get_cartesiantransformationoperator3d(inst: ifcopenshell.entity_instance) -> MatrixType:
    """Parse an IfcCartesianTransformationOperator3D into a 4x4 matrix."""
    values = _capi.placement_get_cartesian_xform_3d(inst._handle)
    return _matrix_from_values(values, fallback="get_cartesiantransformationoperator3d failed to compute a 4x4 matrix")


def get_mappeditem_transformation(item: ifcopenshell.entity_instance) -> Optional[MatrixType]:
    """Parse an IfcMappedItem into a 4x4 transformation matrix.

    Returns ``None`` if the mapping target is 2D (not yet supported natively).
    """
    matrix_values = _capi.placement_get_mappeditem_xform(item._handle)
    if not matrix_values or len(matrix_values) != 16:
        return None
    return _matrix_from_values(matrix_values, fallback="Failed to compute mapped item transformation matrix")


def get_storey_elevation(storey: ifcopenshell.entity_instance) -> float:
    """Return the Z elevation of an IfcBuildingStorey."""
    result = _capi.placement_get_storey_elevation(storey._handle)
    return float(result if result is not None else 0.0)


def rotation(angle: float, axis: Literal["X", "Y", "Z"], is_degrees: bool = True) -> MatrixType:
    """Build a 4x4 rotation matrix around a principal axis."""
    theta = float(np.radians(angle) if is_degrees else angle)
    axis_byte = axis.encode("ascii")[:1] if isinstance(axis, str) else bytes([axis])[:1]
    values = _capi.placement_rotation(theta, axis_byte)
    return _matrix_from_values(values, fallback="rotation failed to compute a 4x4 placement matrix")
