# SPDX-License-Identifier: LGPL-3.0-or-later

"""Placement utilities backed by the native ifcapi C library.

Pure math helpers (``a2p``, ``rotation``) call into C. Entity-aware helpers
(``get_axis2placement``, ``get_local_placement``, etc.) forward to the native
library which walks the IFC graph directly. Results are returned as row-major
4x4 ``numpy.ndarray[float64]`` to match upstream's public contract.
"""

from __future__ import annotations

import ctypes
from typing import Iterable, Literal, Optional

import numpy as np
import numpy.typing as npt

import ifcopenshell
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr

MatrixType = npt.NDArray[np.float64]
"""`npt.NDArray[np.float64]`"""

_bound = False
_PLACEMENT_SYMBOLS = (
    "ifcopenshell_ifcapi_placement_a2p",
    "ifcopenshell_ifcapi_placement_get_axis2placement",
    "ifcopenshell_ifcapi_placement_get_cartesian_xform_3d",
    "ifcopenshell_ifcapi_placement_get_local_placement",
    "ifcopenshell_ifcapi_placement_get_mappeditem_xform",
    "ifcopenshell_ifcapi_placement_get_storey_elevation",
    "ifcopenshell_ifcapi_placement_rotation",
)


def _bind() -> ctypes.CDLL:
    global _bound
    lib = ifcopenshell._get_lib()
    if _bound:
        return lib

    _generated_capi.bind(lib, names=_PLACEMENT_SYMBOLS)
    _bound = True
    return lib


def _matrix_from_values(values: Optional[Iterable[float]]) -> MatrixType:
    if values is None:
        return np.eye(4)
    values = list(values)
    if len(values) != 16:
        return np.eye(4)
    return np.array(values, dtype=np.float64).reshape((4, 4))


def _call_matrix(fn, *args) -> MatrixType:
    return _matrix_from_values(_generated_capi.call_double_list(_bind(), fn, *args))


def a2p(o: Iterable[float], z: Iterable[float], x: Iterable[float]) -> MatrixType:
    """Build a 4x4 transformation matrix from origin, Z axis, and X axis."""
    lib = _bind()
    co = _generated_capi.make_double_list(o)
    cz = _generated_capi.make_double_list(z)
    cx = _generated_capi.make_double_list(x)
    return _call_matrix(
        lib.ifcopenshell_ifcapi_placement_a2p,
        ctypes.byref(co),
        ctypes.byref(cz),
        ctypes.byref(cx),
    )


def get_axis2placement(placement: "ifcopenshell.entity_instance") -> MatrixType:
    """Parse an IfcAxis2Placement (2D, 3D, Linear, or 1D) into a 4x4 matrix."""
    lib = _bind()
    matrix_values = _generated_capi.call_double_list(
        lib,
        lib.ifcopenshell_ifcapi_placement_get_axis2placement,
        _generated_instance_handle_ptr(placement._handle),
    )
    if matrix_values and len(matrix_values) == 16:
        return _matrix_from_values(matrix_values)
    # Fallback for IfcAxis2PlacementLinear with IfcPointByDistanceExpression.
    if placement.is_a("IfcAxis2PlacementLinear"):
        import ifcopenshell.ifcopenshell_wrapper as ifcopenshell_wrapper
        import ifcopenshell.geom
        import ifcopenshell.util.unit

        location = placement.Location
        basis_curve = getattr(location, "BasisCurve", None)
        distance = getattr(location, "DistanceAlong", None)
        if basis_curve is None or distance is None:
            return np.eye(4)

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
    return np.eye(4)


def get_local_placement(placement: Optional["ifcopenshell.entity_instance"] = None) -> MatrixType:
    """Parse an IfcLocalPlacement into a 4x4 transformation matrix."""
    if placement is None:
        return np.eye(4)
    lib = _bind()
    return _call_matrix(
        lib.ifcopenshell_ifcapi_placement_get_local_placement,
        _generated_instance_handle_ptr(placement._handle),
    )


def get_cartesiantransformationoperator3d(inst: "ifcopenshell.entity_instance") -> MatrixType:
    """Parse an IfcCartesianTransformationOperator3D into a 4x4 matrix."""
    lib = _bind()
    return _call_matrix(
        lib.ifcopenshell_ifcapi_placement_get_cartesian_xform_3d,
        _generated_instance_handle_ptr(inst._handle),
    )


def get_mappeditem_transformation(item: "ifcopenshell.entity_instance") -> Optional[MatrixType]:
    """Parse an IfcMappedItem into a 4x4 transformation matrix.

    Returns ``None`` if the mapping target is 2D (not yet supported natively).
    """
    lib = _bind()
    matrix_values = _generated_capi.call_double_list(
        lib,
        lib.ifcopenshell_ifcapi_placement_get_mappeditem_xform,
        _generated_instance_handle_ptr(item._handle),
    )
    if not matrix_values or len(matrix_values) != 16:
        return None
    return _matrix_from_values(matrix_values)


def get_storey_elevation(storey: "ifcopenshell.entity_instance") -> float:
    """Return the Z elevation of an IfcBuildingStorey."""
    lib = _bind()
    result = _generated_capi.call_scalar(
        lib.ifcopenshell_ifcapi_placement_get_storey_elevation,
        ctypes.c_double,
        _generated_instance_handle_ptr(storey._handle),
    )
    return float(result if result is not None else 0.0)


def rotation(angle: float, axis: Literal["X", "Y", "Z"], is_degrees: bool = True) -> MatrixType:
    """Build a 4x4 rotation matrix around a principal axis."""
    lib = _bind()
    theta = float(np.radians(angle) if is_degrees else angle)
    axis_byte = axis.encode("ascii")[:1] if isinstance(axis, str) else bytes([axis])[:1]
    return _call_matrix(lib.ifcopenshell_ifcapi_placement_rotation, theta, axis_byte)
