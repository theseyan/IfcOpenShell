# This file was generated with the assistance of an AI coding tool.
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

MatrixType = npt.NDArray[np.float64]
"""`npt.NDArray[np.float64]`"""

_Mat16 = ctypes.c_double * 16
_Vec3 = ctypes.c_double * 3
_bound = False


def _bind() -> ctypes.CDLL:
    global _bound
    lib = ifcopenshell._get_lib()
    if _bound:
        return lib

    lib.ifcopenshell_placement_a2p.restype = None
    lib.ifcopenshell_placement_a2p.argtypes = [_Vec3, _Vec3, _Vec3, _Mat16]

    lib.ifcopenshell_placement_get_axis2placement.restype = ctypes.c_bool
    lib.ifcopenshell_placement_get_axis2placement.argtypes = [ctypes.c_void_p, _Mat16]

    lib.ifcopenshell_placement_get_local_placement.restype = ctypes.c_bool
    lib.ifcopenshell_placement_get_local_placement.argtypes = [ctypes.c_void_p, _Mat16]

    lib.ifcopenshell_placement_get_cartesian_xform_3d.restype = ctypes.c_bool
    lib.ifcopenshell_placement_get_cartesian_xform_3d.argtypes = [ctypes.c_void_p, _Mat16]

    lib.ifcopenshell_placement_get_mappeditem_xform.restype = ctypes.c_bool
    lib.ifcopenshell_placement_get_mappeditem_xform.argtypes = [ctypes.c_void_p, _Mat16]

    lib.ifcopenshell_placement_get_storey_elevation.restype = ctypes.c_bool
    lib.ifcopenshell_placement_get_storey_elevation.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_double)]

    lib.ifcopenshell_placement_rotation.restype = None
    lib.ifcopenshell_placement_rotation.argtypes = [ctypes.c_double, ctypes.c_char, _Mat16]

    _bound = True
    return lib


def _mat_to_numpy(m: _Mat16) -> MatrixType:
    return np.array(m, dtype=np.float64).reshape((4, 4))


def a2p(o: Iterable[float], z: Iterable[float], x: Iterable[float]) -> MatrixType:
    """Build a 4x4 transformation matrix from origin, Z axis, and X axis."""
    lib = _bind()
    co = _Vec3(*(float(v) for v in o))
    cz = _Vec3(*(float(v) for v in z))
    cx = _Vec3(*(float(v) for v in x))
    out = _Mat16()
    lib.ifcopenshell_placement_a2p(co, cz, cx, out)
    return _mat_to_numpy(out)


def get_axis2placement(placement: "ifcopenshell.entity_instance") -> MatrixType:
    """Parse an IfcAxis2Placement (2D, 3D, Linear, or 1D) into a 4x4 matrix."""
    lib = _bind()
    out = _Mat16()
    ok = lib.ifcopenshell_placement_get_axis2placement(
        placement._handle, out
    )
    if ok:
        return _mat_to_numpy(out)
    # Fallback for IfcAxis2PlacementLinear: evaluate via geom engine.
    if placement.is_a("IfcAxis2PlacementLinear"):
        import ifcopenshell.geom
        settings = ifcopenshell.geom.settings()
        settings.set("convert-back-units", True)
        shape = ifcopenshell.geom.create_shape(settings, placement)
        return np.array(shape.matrix).reshape((4, 4), order="F")
    return np.eye(4)


def get_local_placement(placement: Optional["ifcopenshell.entity_instance"] = None) -> MatrixType:
    """Parse an IfcLocalPlacement into a 4x4 transformation matrix."""
    if placement is None:
        return np.eye(4)
    lib = _bind()
    out = _Mat16()
    lib.ifcopenshell_placement_get_local_placement(placement._handle, out)
    return _mat_to_numpy(out)


def get_cartesiantransformationoperator3d(inst: "ifcopenshell.entity_instance") -> MatrixType:
    """Parse an IfcCartesianTransformationOperator3D into a 4x4 matrix."""
    lib = _bind()
    out = _Mat16()
    lib.ifcopenshell_placement_get_cartesian_xform_3d(inst._handle, out)
    return _mat_to_numpy(out)


def get_mappeditem_transformation(item: "ifcopenshell.entity_instance") -> Optional[MatrixType]:
    """Parse an IfcMappedItem into a 4x4 transformation matrix.

    Returns ``None`` if the mapping target is 2D (not yet supported natively).
    """
    lib = _bind()
    out = _Mat16()
    ok = lib.ifcopenshell_placement_get_mappeditem_xform(item._handle, out)
    if not ok:
        return None
    return _mat_to_numpy(out)


def get_storey_elevation(storey: "ifcopenshell.entity_instance") -> float:
    """Return the Z elevation of an IfcBuildingStorey."""
    lib = _bind()
    out = ctypes.c_double(0.0)
    lib.ifcopenshell_placement_get_storey_elevation(storey._handle, ctypes.byref(out))
    return float(out.value)


def rotation(angle: float, axis: Literal["X", "Y", "Z"], is_degrees: bool = True) -> MatrixType:
    """Build a 4x4 rotation matrix around a principal axis."""
    lib = _bind()
    out = _Mat16()
    theta = float(np.radians(angle) if is_degrees else angle)
    axis_byte = axis.encode("ascii")[:1] if isinstance(axis, str) else bytes([axis])[:1]
    lib.ifcopenshell_placement_rotation(theta, axis_byte, out)
    return _mat_to_numpy(out)
