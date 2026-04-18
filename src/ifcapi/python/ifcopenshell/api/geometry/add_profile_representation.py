# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Any, Literal, Optional, Union, get_args

import ifcopenshell.util.element
import ifcopenshell.util.unit
from ifcopenshell.util.data import Clipping

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
    usecase = Usecase()
    usecase.file = file
    clippings = clippings if clippings is not None else []
    return usecase.execute(context, profile, depth, cardinal_point, clippings, placement_zx_axes)


class Usecase:
    file: ifcopenshell.file
    clippings: list[Clipping]

    def execute(
        self,
        context: ifcopenshell.entity_instance,
        profile: ifcopenshell.entity_instance,
        depth: float,
        cardinal_point: Union[CardinalPoint, None],
        clippings: list[Union[Clipping, dict[str, Any]]],
        placement_zx_axes: tuple[Union[VECTOR_3D, None], Union[VECTOR_3D, None]],
    ) -> ifcopenshell.entity_instance:
        if isinstance(cardinal_point, int):
            cardinal_point = CARDINAL_POINT_VALUES[cardinal_point - 1]

        self.cardinal_point = cardinal_point
        self.profile = profile
        self.clippings = [Clipping.parse(c) for c in clippings]
        self.depth = depth
        self.placement_zx_axes = placement_zx_axes
        self.unit_scale = ifcopenshell.util.unit.calculate_unit_scale(self.file)
        return self.file.create_entity(
            "IfcShapeRepresentation",
            context,
            context.ContextIdentifier,
            "Clipping" if self.clippings else "SweptSolid",
            [self.create_item()],
        )

    def create_item(self) -> ifcopenshell.entity_instance:
        point = self.get_point()
        placement = self.file.createIfcAxis2Placement3D(
            point,
            self.file.create_entity("IfcDirection", self.placement_zx_axes[0] or (0.0, 0.0, 1.0)),
            self.file.create_entity("IfcDirection", self.placement_zx_axes[1] or (1.0, 0.0, 0.0)),
        )
        extrusion = self.file.create_entity(
            "IfcExtrudedAreaSolid",
            self.profile,
            placement,
            self.file.createIfcDirection((0.0, 0.0, 1.0)),
            self.convert_si_to_unit(self.depth),
        )
        if self.clippings:
            return self.apply_clippings(extrusion)
        return extrusion

    def apply_clippings(self, first_operand: ifcopenshell.entity_instance) -> ifcopenshell.entity_instance:
        while self.clippings:
            clipping = self.clippings.pop()
            if isinstance(clipping, ifcopenshell.entity_instance):
                new = ifcopenshell.util.element.copy(self.file, clipping)
                new.FirstOperand = first_operand
                first_operand = new
            else:  # Clipping
                first_operand = clipping.apply(self.file, first_operand, self.unit_scale)
        return first_operand

    def convert_si_to_unit(self, co: float) -> float:
        return co / self.unit_scale

    def get_point(self) -> ifcopenshell.entity_instance:
        if not self.cardinal_point:
            return self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))
        elif self.cardinal_point == "bottom left":
            return self.file.createIfcCartesianPoint((-self.get_x() / 2, self.get_y() / 2, 0.0))
        elif self.cardinal_point == "bottom centre":
            return self.file.createIfcCartesianPoint((0.0, self.get_y() / 2, 0.0))
        elif self.cardinal_point == "bottom right":
            return self.file.createIfcCartesianPoint((self.get_x() / 2, self.get_y() / 2, 0.0))
        elif self.cardinal_point == "mid-depth left":
            return self.file.createIfcCartesianPoint((-self.get_x() / 2, 0.0, 0.0))
        elif self.cardinal_point == "mid-depth centre":
            return self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))
        elif self.cardinal_point == "mid-depth right":
            return self.file.createIfcCartesianPoint((self.get_x() / 2, 0.0, 0.0))
        elif self.cardinal_point == "top left":
            return self.file.createIfcCartesianPoint((-self.get_x() / 2, -self.get_y() / 2, 0.0))
        elif self.cardinal_point == "top centre":
            return self.file.createIfcCartesianPoint((0.0, -self.get_y() / 2, 0.0))
        elif self.cardinal_point == "top right":
            return self.file.createIfcCartesianPoint((self.get_x() / 2, -self.get_y() / 2, 0.0))
        # TODO other cardinal points
        return self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))

    def _extents(self) -> tuple[float, float]:
        import ctypes
        from ifcopenshell import _get_lib
        lib = _get_lib()
        lib.ifcopenshell_geometry_profile_extents.restype = ctypes.c_bool
        lib.ifcopenshell_geometry_profile_extents.argtypes = [
            ctypes.c_void_p, ctypes.c_void_p,
            ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
        ]
        x = ctypes.c_double(0.0)
        y = ctypes.c_double(0.0)
        lib.ifcopenshell_geometry_profile_extents(
            self.file._ptr, self.profile._handle, ctypes.byref(x), ctypes.byref(y)
        )
        return x.value, y.value

    def get_x(self) -> float:
        return self._extents()[0]

    def get_y(self) -> float:
        return self._extents()[1]
