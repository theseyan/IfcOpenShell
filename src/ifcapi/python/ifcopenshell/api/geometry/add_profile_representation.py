# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
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

from typing import Any, Literal, Optional, Union, get_args

import ifcopenshell
from ifcopenshell.util.data import Clipping

from . import _capi

VECTOR_3D = tuple[float, float, float]
CardinalPointNumeric = Literal[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
CardinalPointString = Literal[
    "bottom left",
    "bottom centre",
    "bottom right",
    "mid-depth left",
    "mid-depth centre",
    "mid-depth right",
    "top left",
    "top centre",
    "top right",
    "geometric centroid",
    "bottom in line with the geometric centroid",
    "left in line with the geometric centroid",
    "right in line with the geometric centroid",
    "top in line with the geometric centroid",
    "shear centre",
    "bottom in line with the shear centre",
    "left in line with the shear centre",
    "right in line with the shear centre",
    "top in line with the shear centre",
]
CARDINAL_POINT_VALUES: tuple[CardinalPointString, ...] = get_args(CardinalPointString)
CardinalPoint = Union[CardinalPointNumeric, CardinalPointString]


def add_profile_representation(
    file: ifcopenshell.file,
    context: ifcopenshell.entity_instance,
    profile: ifcopenshell.entity_instance,
    depth: float = 1.0,
    # TODO: None makes more sense as default value?
    cardinal_point: Union[CardinalPoint, None] = 5,
    clippings: Optional[list[Union[Clipping, dict[str, Any]]]] = None,
    placement_zx_axes: tuple[Union[VECTOR_3D, None], Union[VECTOR_3D, None]] = (None, None),
) -> ifcopenshell.entity_instance:
    """Add profile representation.

    :param context: The IfcGeometricRepresentationContext for the representation,
        only Model/Body/MODEL_VIEW type of representations are currently supported.
    :param profile: The IfcProfileDef to extrude.
    :param depth: The depth of the extrusion in meters.
    :param cardinal_point: The cardinal point of the profile.
    :param clippings: A list of planes that define clipping half space solids.
        Planes are defined either by Clipping objects
        or by dictionaries of arguments for `Clipping.parse`.
    :param placement_zx_axes: A tuple of two vectors that define the placement of the profile.
        The first vector is the Z axis, the second vector is the X axis.
    :return: IfcShapeRepresentation.
    """
    if isinstance(cardinal_point, int):
        cardinal_point = CARDINAL_POINT_VALUES[cardinal_point - 1]

    clipping_kinds: list[int] = []
    clipping_locations: list[tuple[float, float, float]] = []
    clipping_normals: list[tuple[float, float, float]] = []
    clipping_entities: list[ifcopenshell.entity_instance] = []
    for clipping in clippings if clippings is not None else []:
        parsed = Clipping.parse(clipping)
        if isinstance(parsed, ifcopenshell.entity_instance):
            clipping_kinds.append(1)
            clipping_entities.append(parsed)
        else:
            clipping_kinds.append(0)
            clipping_locations.append(parsed.location)
            clipping_normals.append(parsed.normal)

    return _capi.call_handle(
        file,
        "geometry_add_profile_representation",
        _capi.file_handle(file),
        {
            "context": _capi.instance_handle(context),
            "profile": _capi.instance_handle(profile),
            "depth": depth,
            "cardinal_point": cardinal_point,
            "placement_z_axis": placement_zx_axes[0],
            "placement_x_axis": placement_zx_axes[1],
            "clipping_kinds": clipping_kinds,
            "clipping_locations": clipping_locations,
            "clipping_normals": clipping_normals,
            "clipping_entities": _capi.instance_list(clipping_entities),
        },
    )
