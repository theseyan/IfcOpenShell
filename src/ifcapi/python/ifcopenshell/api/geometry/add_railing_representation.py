# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2023 @Andrej730
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

from dataclasses import dataclass, field
from typing import Literal, Optional

import numpy as np

import ifcopenshell
from ifcopenshell.util.shape_builder import SequenceOfVectors

from . import _capi

TERMINAL_TYPE = Literal[
    "180",
    "TO_END_POST",
    "TO_WALL",
    "TO_FLOOR",
    "TO_END_POST_AND_FLOOR",
    "NONE",
]


@dataclass(slots=True)
class RailingSupport:
    """Pure geometry for one wall-mounted handrail support, in project units."""

    arc_polyline: np.ndarray
    arc_radius: float
    disk_position: np.ndarray
    disk_radius: float
    disk_depth: float
    disk_z_rotation: float


@dataclass(slots=True)
class WallMountedHandrailGeometry:
    """Pure wall-mounted handrail geometry, independent of an IFC file."""

    handrail_polyline: np.ndarray
    handrail_arc_point_indices: list[int]
    handrail_radius: float
    supports: list[RailingSupport] = field(default_factory=list)


def _support_from_native(value) -> RailingSupport:
    return RailingSupport(
        arc_polyline=np.asarray(value.arc_polyline, dtype=float),
        arc_radius=float(value.arc_radius),
        disk_position=np.asarray(value.disk_position, dtype=float),
        disk_radius=float(value.disk_radius),
        disk_depth=float(value.disk_depth),
        disk_z_rotation=float(value.disk_z_rotation),
    )


def compute_wall_mounted_handrail_geometry(
    *,
    railing_path: SequenceOfVectors,
    support_spacing: float,
    railing_diameter: float,
    clear_width: float,
    height: float,
    use_manual_supports: bool = False,
    terminal_type: TERMINAL_TYPE = "180",
    looped_path: bool = False,
    unit_scale: float = 1.0,
) -> WallMountedHandrailGeometry:
    """Compute wall-mounted handrail geometry without creating IFC entities.

    Inputs and outputs use IFC project units. ``unit_scale`` converts only the
    fixed metric terminal, fillet, support-arc, and attachment-depth constants.
    """
    result = _capi.call_value(
        "geometry_compute_wall_mounted_handrail_geometry",
        {
            "railing_path": railing_path,
            "support_spacing": support_spacing,
            "railing_diameter": railing_diameter,
            "clear_width": clear_width,
            "height": height,
            "use_manual_supports": use_manual_supports,
            "terminal_type": terminal_type,
            "looped_path": looped_path,
            "unit_scale": unit_scale,
        },
    )
    return WallMountedHandrailGeometry(
        handrail_polyline=np.asarray(result.handrail_polyline, dtype=float),
        handrail_arc_point_indices=list(result.handrail_arc_point_indices),
        handrail_radius=float(result.handrail_radius),
        supports=[_support_from_native(support) for support in result.supports],
    )


def add_railing_representation(
    file: ifcopenshell.file,
    *,
    context: ifcopenshell.entity_instance,
    railing_path: SequenceOfVectors,
    use_manual_supports: bool = False,
    support_spacing: Optional[float] = None,
    railing_diameter: Optional[float] = None,
    clear_width: Optional[float] = None,
    terminal_type: TERMINAL_TYPE = "180",
    height: Optional[float] = None,
    looped_path: bool = False,
    unit_scale: Optional[float] = None,
) -> ifcopenshell.entity_instance:
    """Create an IFC wall-mounted handrail representation.

    Explicit dimensions and coordinates use project units. Omitted dimensions
    use the current upstream metric defaults after the file unit scale is
    resolved. Passing ``railing_path=None`` deliberately retains the upstream
    default three-point path despite the public required-path annotation.
    """
    options = {
        "context": _capi.instance_handle(context),
        "use_manual_supports": use_manual_supports,
        "terminal_type": terminal_type,
        "looped_path": looped_path,
    }
    if railing_path is not None:
        options["railing_path"] = railing_path
    if support_spacing is not None:
        options["support_spacing"] = support_spacing
    if railing_diameter is not None:
        options["railing_diameter"] = railing_diameter
    if clear_width is not None:
        options["clear_width"] = clear_width
    if height is not None:
        options["height"] = height
    if unit_scale is not None:
        options["unit_scale"] = unit_scale
    return _capi.call_handle(
        file,
        "geometry_add_railing_representation",
        _capi.file_handle(file),
        options,
    )
