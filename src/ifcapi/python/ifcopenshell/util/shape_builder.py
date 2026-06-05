# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022, 2023 @Andrej730
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

from __future__ import annotations

import collections.abc
from collections.abc import Sequence
from itertools import chain
from math import cos, pi, radians
from typing import TYPE_CHECKING, Any, Literal, Optional, Union

import numpy as np
import numpy.typing as npt

import ifcopenshell
import ifcopenshell.util.placement
import ifcopenshell.util.representation
from ifcopenshell import _ifcopenshell_capi as _capi

PRECISION = 1.0e-5


if TYPE_CHECKING:
    # NOTE: mathutils is never used at runtime in ifcopenshell,
    # only for type checking to ensure methods are compatible with
    # Blender vectors.
    from mathutils import (
        Vector,  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]
    )

    # Support both numpy arrays and python sequences as inputs.
    VectorType = Union[Sequence[float], Vector, np.ndarray]
else:
    # Ensure it's exportable, so other modules can reuse it for typing.
    VectorType = Any

SequenceOfVectors = Union[Sequence[VectorType], np.ndarray]

def _native_entity(file: ifcopenshell.file, symbol: str, *args) -> ifcopenshell.entity_instance:
    handle = getattr(_capi, symbol)(file._handle, *args)
    if handle is None:
        raise RuntimeError(f"{symbol} returned a null Ifc instance handle")
    return ifcopenshell.entity_instance(file, handle)


def _native_entity_from_struct_handle(
    file: ifcopenshell.file, handle, symbol: str
) -> ifcopenshell.entity_instance:
    if handle is None:
        raise RuntimeError(f"{symbol} returned a null Ifc instance handle")
    return ifcopenshell.entity_instance(file, handle)


def _native_points(points: SequenceOfVectors) -> SequenceOfVectors:
    return points


def _native_vector(vector: VectorType) -> VectorType:
    return vector


def _native_instance_list(values: Sequence[ifcopenshell.entity_instance]) -> list:
    return [value._handle for value in values]


def _native_faces(faces: Sequence[Sequence[int]]) -> Sequence[Sequence[int]]:
    return faces


def _curve_dim(curve: ifcopenshell.entity_instance) -> int | None:
    try:
        return curve.Dim
    except AttributeError:
        pass
    if curve.is_a("IfcTrimmedCurve"):
        return _curve_dim(curve.BasisCurve)
    if curve.is_a("IfcCompositeCurve") and curve.Segments:
        return _curve_dim(curve.Segments[0].ParentCurve)
    if curve.is_a("IfcPolyline") and curve.Points:
        return len(curve.Points[0].Coordinates)
    if curve.is_a("IfcIndexedPolyCurve") and curve.Points and curve.Points.CoordList:
        return len(curve.Points.CoordList[0])
    if hasattr(curve, "Position") and curve.Position and curve.Position.Location:
        return len(curve.Position.Location.Coordinates)
    return None


def _native_polygonal_faces(
    faces: Sequence[Union[Sequence[int], Sequence[Sequence[int]]]]
) -> list[Union[Sequence[int], Sequence[Sequence[int]]]]:
    def is_sequence_of_ints(x):
        return isinstance(x, Sequence) and not isinstance(x, (str, bytes)) and all(isinstance(el, int) for el in x)

    def is_sequence_of_sequence_of_ints(x):
        return isinstance(x, Sequence) and not isinstance(x, (str, bytes)) and all(is_sequence_of_ints(el) for el in x)

    if not all(is_sequence_of_ints(f) or is_sequence_of_sequence_of_ints(f) for f in faces):
        raise ValueError("Expected a sequence of int or sequence of sequence of int for each face")

    return [[face] if is_sequence_of_ints(face) else face for face in faces]  # type: ignore[list-item]


def _struct_field(value: Any, name: str, index: int) -> Any:
    if hasattr(value, name):
        return getattr(value, name)
    return value[index]


def V(*args: Union[float, int, VectorType, SequenceOfVectors]) -> npt.NDArray[np.float64]:
    """Convert floats / vector / sequence of vectors to numpy array.

    Note that `float` argument type also allows passing ints,
    which will be converted to floats (a double type) as IfcOpenShell is strict
    about setting int/float attributes.
    """
    if isinstance(args[0], (float, int)):
        return np.array(args, dtype="d")

    assert len(args) == 1, "Only single argument is supported if providing a vector or a sequence of them."
    return np.array(args[0], dtype="d")


def ifc_safe_vector_type(v: Union[VectorType, SequenceOfVectors]) -> Any:
    """Convert vector / sequence of vectors to a list of floats
    that's safe to save IFC attribute.

    Basically converting all numbers in sequences to Python floats.
    """
    return np.array(v, dtype="d").tolist()


def is_x(value: float, x: float, si_conversion: Optional[float] = None) -> bool:
    if si_conversion is not None:
        value = value * si_conversion
    return (x + PRECISION) > value > (x - PRECISION)


def round_to_precision(x: float, si_conversion: float) -> float:
    return round(x * si_conversion, 5) / si_conversion


def np_round_to_precision(v: np.ndarray, si_conversion: float) -> np.ndarray:
    return np.round(v * si_conversion, 5) / si_conversion


def np_normalized(v: VectorType) -> np.ndarray:
    return np.divide(v, np.linalg.norm(v))


def np_matrix_normalized(matrix: np.ndarray) -> np.ndarray:
    # Ensure translation is not affected.
    scale_factors = np.linalg.norm(matrix[:3, :3], axis=0)
    rotation_matrix = matrix.copy()
    rotation_matrix[:3, :3] /= scale_factors
    return rotation_matrix


def np_lerp(a: VectorType, b: VectorType, t: float) -> np.ndarray:
    return a + np.subtract(b, a) * t


def np_to_3d(v: VectorType, z: float = 0.0) -> np.ndarray:
    """Convert 2D/4D vector to 3D."""
    l = len(v)
    if l == 2:
        return np.append(v, z)
    elif l == 4:
        return v[:3]
    assert False, f"Unexpected vector length: {l} ({v})."


def np_to_4d(v: VectorType, z: float = 0.0, w: float = 1.0) -> np.ndarray:
    """Convert 2D/3D vector to 4D (e.g. for multiplying with 4x4 matrix)."""
    l = len(v)
    if l == 2:
        return np.append(v, (z, w))
    elif l == 3:
        return np.append(v, w)
    assert False, f"Unexpected vector length: {l} ({v})."


def np_to_4x4(matrix_3x3: np.ndarray) -> np.ndarray:
    """Convert 3x3 matrix to 4x4."""
    matrix_4x4 = np.pad(matrix_3x3, ((0, 1), (0, 1)))
    matrix_4x4[3, 3] = 1
    return matrix_4x4


def np_apply_matrix(vectors: SequenceOfVectors, matrix: npt.NDArray) -> npt.NDArray:
    """
    :param vectors: Nx3 array of vectors.
    :param matrix: 4x4 transformation matrix.
    """
    m3x3 = matrix[:3, :3]
    translation = matrix[:3, 3]
    return vectors @ m3x3.T + translation


def np_angle(a: VectorType, b: VectorType) -> float:
    """Get angle between vectors in radians.
    Designed to work similar to `Vector.angle`.
    """
    return np.arccos(np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b)))


def np_angle_signed(a: VectorType, b: VectorType) -> float:
    """Get signed angle between 2D vectors in radians (clockwise is positive).
    Designed to work similar to `Vector.angle_signed`.
    """
    assert len(a) == 2 and len(b) == 2, "Only 2D vectors are supported."
    det = a[1] * b[0] - a[0] * b[1]
    dot = np.dot(a, b)
    return np.arctan2(det, dot)


def np_translation_matrix(vector: VectorType) -> npt.NDArray[np.float64]:
    """Get translation matrix.

    Designed to be similar to mathutils Matrix.Rotation but to use numpy.

    :param vector: 3D translation vector.
    :return: An 4x4 identity matrix with a translation
    """
    eye = np.eye(4, dtype=np.float64)
    M_TRANSLATION = (slice(0, 3), 3)
    eye[M_TRANSLATION] = vector
    return eye


def np_rotation_matrix(
    angle: float, size: int, axis: Optional[Union[Literal["X", "Y", "Z"], VectorType]] = None
) -> np.ndarray:
    """Get rotation matrix. Designed to be similar to mathutils Matrix.Rotation but to use numpy.

    :param float: Rotation angle, in radians.
    :param size: Matrix size ([2;4]).
    :param axis: Rotation axis.
        For 2x2 matrices Z assumed by default and argument can be omitted,
        for 3x3/4x4 matrices could be either axis literal
        or a rotation axis presented as a vector.
    :return: Rotation matrix.
    """
    if not (2 <= size <= 4):
        raise ValueError(f"Size must be [2;4], got {size}.")

    cos_theta: float = np.cos(angle)
    sin_theta: float = np.sin(angle)
    if size == 2:
        return np.array([[cos_theta, -sin_theta], [sin_theta, cos_theta]])

    assert axis, "For non-2D matrices 'axis' argument is not optional."
    if isinstance(axis, str):
        if axis == "X":
            matrix = np.array([[1, 0, 0], [0, cos_theta, -sin_theta], [0, sin_theta, cos_theta]])
        elif axis == "Y":
            matrix = np.array([[cos_theta, 0, sin_theta], [0, 1, 0], [-sin_theta, 0, cos_theta]])
        elif axis == "Z":
            matrix = np.array([[cos_theta, -sin_theta, 0], [sin_theta, cos_theta, 0], [0, 0, 1]])
    else:
        # Assume axis is a vector.
        axis = axis / np.linalg.norm(axis)
        # Rodrigues' rotation formula.
        K = np.array([[0, -axis[2], axis[1]], [axis[2], 0, -axis[0]], [-axis[1], axis[0], 0]])
        matrix = cos_theta * np.eye(3) + (1 - cos_theta) * np.outer(axis, axis) + sin_theta * K
    if size == 4:
        return np_to_4x4(matrix)
    return matrix


def np_matrix_to_euler(matrix: np.ndarray) -> tuple[float, float, float]:
    """Convert a rotation matrix to Euler angles.

    Designed to work similar to `mathutils.Matrix.to_euler`.
    Currently only XYZ rotation is supported.
    """
    if matrix.shape not in ((3, 3), (4, 4)):
        raise ValueError(f"Matrix must be 3x3 or 4x4, got {matrix.shape}.")

    matrix = np_matrix_normalized(matrix)
    y = -np.arcsin(matrix[2, 0])
    cos_y = np.cos(y)
    x = np.arctan2(matrix[2, 1] / cos_y, matrix[2, 2] / cos_y)
    z = np.arctan2(matrix[1, 0] / cos_y, matrix[0, 0] / cos_y)
    return (x, y, z)


def np_normal(vectors: SequenceOfVectors) -> np.ndarray:
    """Normal of 3D Polygon.

    Designed to work similar to `mathutils.geometry.normal`.
    """
    assert len(vectors) == 3, "3 vectors required"
    # TODO: can be optimized?
    verts_np = np.array(vectors[:3])
    v0, v1, v2 = verts_np[:3]
    edge1 = v1 - v0
    edge2 = v2 - v0
    normal = np.cross(edge1, edge2)
    norm = np.linalg.norm(normal)
    return normal / norm


def np_intersect_line_line(
    v1: VectorType, v2: VectorType, v3: VectorType, v4: VectorType
) -> tuple[np.ndarray, np.ndarray]:
    """Get 2 closest points on each line.
    First line - (v1, v2). Second line - (v3, v4).

    Designed to work similar to `mathutils.geometry.intersect_line_line`.
    """
    # TODO: could be optimized?
    d1 = np.subtract(v2, v1)
    d2 = np.subtract(v4, v3)

    # Cross product of the directions
    cross_d1_d2 = np.cross(d1, d2)
    cross_d1_d2_norm: float = np.linalg.norm(cross_d1_d2)

    # Check if the lines are parallel.
    if is_x(cross_d1_d2_norm, 0):
        raise ValueError("Lines are parallel and do not intersect uniquely.")

    r = np.subtract(v3, v1)
    t = np.dot(np.cross(r, d2), cross_d1_d2) / (cross_d1_d2_norm**2)
    u = np.dot(np.cross(r, d1), cross_d1_d2) / (cross_d1_d2_norm**2)

    # Closest points on each line
    point_on_line1 = v1 + t * d1
    point_on_line2 = v3 + u * d2
    return point_on_line1, point_on_line2


def intersect_x_axis_2d(p1: VectorType, p2: VectorType, y=0) -> Optional[float]:
    """Intersect a line defined by 2 points to a horizontal line defined by y

    Useful for axis-aligned intersection checks.

    :param p1: First 2D point of the line, order doesn't matter
    :param p2: Second 2D point of the line, order doesn't matter
    :param y: Intersect at this y value (i.e. defaults to y=0)
    """
    x1, y1 = p1
    x2, y2 = p2
    if is_x(y1, y2):  # Parallel
        return
    t = (y - y1) / (y2 - y1)
    return x1 + t * (x2 - x1)


# Note: using ShapeBuilder try not to reuse IFC elements in the process
# otherwise you might run into situation where builder.mirror or other operation
# is applied twice during one run to the same element
# which might produce undesirable results


class ShapeBuilder:
    def __init__(self, ifc_file: ifcopenshell.file):
        self.file = ifc_file

    def polyline(
        self,
        points: SequenceOfVectors,
        closed: bool = False,
        position_offset: Optional[VectorType] = None,
        arc_points: Sequence[int] = (),
    ) -> ifcopenshell.entity_instance:
        """
        Generate an IfcIndexedPolyCurve based on the provided points.

        :param points: List of 2d or 3d points
        :param closed: Whether polyline should be closed.
        :param position_offset: offset to be applied to all points
        :param arc_points: Indices of the middle points for arcs. For creating an arc segment,
            provide 3 points: `arc_start`, `arc_middle` and `arc_end` to `points` and add the `arc_middle`
            point's index to `arc_points`
        :return: IfcIndexedPolyCurve

        Example:

        .. code:: python

            # rectangle
            points = Vector((0, 0)), Vector((1, 0)), Vector((1, 1)), Vector((0, 1))
            position = Vector((2, 0))
            # #2=IfcIndexedPolyCurve(#1,(IfcLineIndex((1,2,3,4,1))),$)
            polyline = builder.polyline(points, closed=True, position_offset=position)

            # arc between points (1,0) and (0,1). Second point in the arc should be it's middle
            points = Vector((1, 0)), Vector((0.707, 0.707)), Vector((0, 1)), Vector((0,2))
            arc_points = (1,) # point with index 1 is a middle of the arc
            # 4=IfcIndexedPolyCurve(#3,(IfcArcIndex((1,2,3)),IfcLineIndex((3,4,1))),$)
            curved_polyline = builder.polyline(points, closed=False, position_offset=position, arc_points=arc_points)
        """

        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_polyline",
            _native_points(points),
            bool(closed),
            _native_vector(position_offset if position_offset is not None else ()),
            position_offset is not None,
            arc_points,
        )

    @staticmethod
    def get_rectangle_coords(size: VectorType = (1.0, 1.0), position: Optional[VectorType] = None) -> np.ndarray:
        """
        Get rectangle coords arranged as below:

        ::

            3 2
            0 1

        :param size: rectangle size, could be either 2d or 3d.
            Use 0 for one of 3d dimensions to create 2d rectangle in 3d space.
        :param position: rectangle position.
            if `position` not specified zero-vector will be used
        :return: list of rectangle coords
        """
        size_np = np.array(size)

        if position is None:
            dimensions = len(size_np)
            points = np.full((4, dimensions), 0.0)
        else:
            points = np.tile(position, (4, 1))

        # Support both 2d and 3d sizes defined in different dimensions.
        non_empty_coords = np.nonzero(size_np)[0]
        points[1, non_empty_coords[0]] += size_np[non_empty_coords[0]]
        points[2] += size_np
        points[3, non_empty_coords[1]] += size_np[non_empty_coords[1]]
        return points

    def rectangle(
        self, size: VectorType = (1.0, 1.0), position: Optional[VectorType] = None
    ) -> ifcopenshell.entity_instance:
        """
        Generate a rectangle polyline.

        :param size: rectangle.
        :param position: rectangle position.

        See ``get_rectangle_coords`` for more information.

        :return: IfcIndexedPolyCurve
        """
        return self.polyline(self.get_rectangle_coords(size, position), closed=True)

    def circle(self, center: VectorType = (0.0, 0.0), radius: float = 1.0) -> ifcopenshell.entity_instance:
        """
        :param center: circle 2D position
        :param radius: radius of the circle
        :return: IfcCircle
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_circle",
            _native_vector(center),
            float(radius),
        )

    def plane(
        self, location: VectorType = (0.0, 0.0, 0.0), normal: VectorType = (0.0, 0.0, 1.0)
    ) -> ifcopenshell.entity_instance:
        """
        Create IfcPlane.

        :param location: plane position.
        :param normal: plane normal direction.
        :return: IfcPlane
        """

        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_plane",
            _native_vector(location),
            _native_vector(normal),
        )

    # TODO: explain points order for the curve_between_two_points
    # because the order is important and defines the center of the curve
    # currently it seems like the first point shifted by x-axis defines the center
    def curve_between_two_points(self, points: tuple[VectorType, VectorType]) -> ifcopenshell.entity_instance:
        """Simple circle based curve between two points
        Good for creating curves and fillets, won't work for continuous ellipse shapes.

        :param points: tuple of 2 points.
        :return: IfcIndexePolyCurve
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_curve_between_two_points",
            _native_points(points),
        )

    def get_trim_points_from_mask(
        self,
        x_axis_radius: float,
        y_axis_radius: float,
        trim_points_mask: Sequence[int],
        position_offset: Optional[VectorType] = None,
    ) -> np.ndarray:
        """Get cardinal-point coordinates of an ellipse by index mask.

        The four cardinal points are numbered 0–3 counter-clockwise starting from the
        positive X axis: 0 → ``(x, 0)``, 1 → ``(0, y)``, 2 → ``(-x, 0)``, 3 → ``(0, -y)``.

        Example: mask ``(0, 1, 2, 3)`` returns all four points in order.

        :param x_axis_radius: Radius (semi-axis length) along the X axis.
        :param y_axis_radius: Radius (semi-axis length) along the Y axis.
        :param trim_points_mask: Sequence of cardinal-point indices (0–3) to select.
        :param position_offset: Optional 2D offset added to all returned points.
        :return: Numpy array of the selected 2D points.
        """
        points = np.array(
            (
                (x_axis_radius, 0),
                (0, y_axis_radius),
                (-x_axis_radius, 0),
                (0, -y_axis_radius),
            )
        )
        # list type is important for selecting items by the indices.
        trim_points = points[list(trim_points_mask)]
        if position_offset is None:
            return trim_points
        return trim_points + position_offset

    def create_ellipse_curve(
        self,
        x_axis_radius: float,
        y_axis_radius: float,
        position: VectorType = (0.0, 0.0),
        trim_points: SequenceOfVectors = (),
        ref_x_direction: VectorType = (1.0, 0.0),
        trim_points_mask: Sequence[int] = (),
    ) -> ifcopenshell.entity_instance:
        """Create an IfcEllipse, optionally trimmed to an arc.

        If neither ``trim_points`` nor ``trim_points_mask`` is provided, a full IfcEllipse is returned.
        Trimming points must be given in counter-clockwise order. For example, to get the arc
        above the Y-axis use mask ``(0, 2)``; below the Y-axis use ``(2, 0)``.

        A trimmed result (IfcTrimmedCurve) includes a closing segment between the trim points,
        making it suitable for use as a profile in :meth:`extrude`.

        :param x_axis_radius: Semi-axis length along the local X axis.
        :param y_axis_radius: Semi-axis length along the local Y axis.
        :param position: 2D centre of the ellipse.
        :param trim_points: Explicit pair of 2D trim points. Takes precedence over ``trim_points_mask``.
        :param ref_x_direction: Direction of the local X axis.
        :param trim_points_mask: Pair of cardinal-point indices (0–3) used when ``trim_points`` is empty.
            See :meth:`get_trim_points_from_mask` for index definitions.
        :return: IfcEllipse (untrimmed) or IfcTrimmedCurve (trimmed).
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_ellipse_curve",
            x_axis_radius,
            y_axis_radius,
            _native_vector(position),
            _native_points(trim_points),
            _native_vector(ref_x_direction),
            trim_points_mask,
        )

    def profile(
        self,
        outer_curve: ifcopenshell.entity_instance,
        name: Optional[str] = None,
        inner_curves: Sequence[ifcopenshell.entity_instance] = (),
        profile_type: str = "AREA",
    ) -> ifcopenshell.entity_instance:
        """Create a profile.

        :param outer_curve: Profile IfcCurve.
        :param inner_curves: a sequence of IfcCurves.

        :return: IfcArbitraryClosedProfileDef or IfcArbitraryProfileDefWithVoids.
        """
        # inner_curves could be used as a tool for boolean operation
        # but if any point of inner curve will go outside the outer curve
        # it will just add shape on top instead of "boolean" it
        # because of that you can't create bool edges of outer_curve this way

        outer_curve_dim = _curve_dim(outer_curve)
        if outer_curve_dim != 2:
            raise Exception(
                f"Outer curve for IfcArbitraryClosedProfileDef/IfcIfcArbitraryProfileDefWithVoid should be 2D to be valid, currently it has {outer_curve_dim} dimensions.\n"
                "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcArbitraryClosedProfileDef.htm#8.15.3.1.4-Formal-propositions"
            )

        if inner_curves:
            if not isinstance(inner_curves, collections.abc.Iterable):
                inner_curves = [inner_curves]
            if any(_curve_dim(curve) != 2 for curve in inner_curves):
                raise Exception(
                    "WARNING. InnerCurve for IfcIfcArbitraryProfileDefWithVoid sould be 2D to be valid, "
                    "currently on one of the inner curves is using different amount of dimensions.\n"
                    "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcArbitraryClosedProfileDef.htm#8.15.3.1.4-Formal-propositions"
                )

        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_profile",
            outer_curve._handle,
            name,
            _native_instance_list(inner_curves),
            profile_type,
        )

    def translate(
        self,
        curve_or_item: Union[ifcopenshell.entity_instance, Sequence[ifcopenshell.entity_instance]],
        translation: VectorType,
        create_copy: bool = False,
    ) -> Union[ifcopenshell.entity_instance, list[ifcopenshell.entity_instance]]:
        """
        Translate curve/representaiton item/representation.

        :param curve_or_item: A single item to translate or a sequence of them.
        :param translation: Translation vector.
        :param create_copy: Whether to translate the provided item or it's copy.
        :return: Translated curve/item/representation or a sequence of them.
        """

        multiple_objects = isinstance(curve_or_item, collections.abc.Iterable)
        if not multiple_objects:
            curve_or_item = [curve_or_item]

        processed_objects: list[ifcopenshell.entity_instance] = []
        for c in curve_or_item:
            processed_objects.append(
                _native_entity(
                    self.file,
                    "ifcopenshell_ifcapi_shape_builder_translate",
                    c._handle,
                    _native_vector(translation),
                    bool(create_copy),
                )
            )

        return processed_objects if multiple_objects else processed_objects[0]

    def rotate_2d_point(
        self,
        point_2d: VectorType,
        angle: float = 90.0,
        pivot_point: VectorType = (0.0, 0.0),
        counter_clockwise: bool = False,
    ) -> np.ndarray:
        """Rotate a single 2D point around a pivot.

        :param point_2d: The 2D point to rotate.
        :param angle: Rotation angle, in degrees. Defaults to 90.
        :param pivot_point: The point to rotate around.
        :param counter_clockwise: If True, rotate counter-clockwise. Defaults to clockwise.
        :return: Rotated 2D point as a numpy array.
        """
        angle_rad = radians(angle) * (1 if counter_clockwise else -1)
        relative_point = np.array(point_2d) - pivot_point
        relative_point = np_rotation_matrix(angle_rad, 2) @ relative_point
        final_point = relative_point + pivot_point
        return final_point

    def rotate(
        self,
        curve_or_item: Union[ifcopenshell.entity_instance, Sequence[ifcopenshell.entity_instance]],
        angle: float = 90.0,
        pivot_point: VectorType = (0.0, 0.0),
        counter_clockwise: bool = False,
        create_copy: bool = False,
    ) -> Union[ifcopenshell.entity_instance, list[ifcopenshell.entity_instance]]:
        """Rotate curve/representaiton item/representation.

        :param curve_or_item: A single item to rotate or a sequence of them.
        :param angle: Rotation angle, in degrees.
        :param pivot_point: Rotation pivot point.
        :param counter_clockwise: Whether rotation is counter-clockwise.
        :param create_copy: Whether to rotate the provided item or it's copy.
        :return: Rotated curve/representaiton item/representation or a sequence of them.
        """

        multiple_objects = isinstance(curve_or_item, collections.abc.Iterable)
        if not multiple_objects:
            curve_or_item = [curve_or_item]

        processed_objects: list[ifcopenshell.entity_instance] = []
        for c in curve_or_item:
            processed_objects.append(
                _native_entity(
                    self.file,
                    "ifcopenshell_ifcapi_shape_builder_rotate",
                    c._handle,
                    float(angle),
                    _native_vector(pivot_point),
                    bool(counter_clockwise),
                    bool(create_copy),
                )
            )

        return processed_objects if multiple_objects else processed_objects[0]

    def mirror_2d_point(
        self,
        point_2d: VectorType,
        mirror_axes: VectorType = (1.0, 1.0),
        mirror_point: VectorType = (0.0, 0.0),
    ) -> np.ndarray:
        """Mirror a single 2D point across the specified axes.

        :param point_2d: The 2D point to mirror.
        :param mirror_axes: Indicates which axes to mirror across. A positive value in a
            component means that axis is mirrored (negated relative to ``mirror_point``).
            Example: ``(1, 0)`` mirrors across the Y-axis (negates X only),
            ``(1, 1)`` mirrors across both axes.
        :param mirror_point: Origin of the mirror operation.
        :return: Mirrored 2D point as a numpy array.
        """
        mirror_axes: np.ndarray = np.where(np.array(mirror_axes) > 0, -1, 1)
        mirror_point: np.ndarray = np.array(mirror_point)
        relative_point = point_2d - mirror_point
        relative_point = relative_point * mirror_axes
        point_2d_res = relative_point + mirror_point
        return point_2d_res

    def create_axis2_placement_3d(
        self,
        position: VectorType = (0.0, 0.0, 0.0),
        z_axis: VectorType = (0.0, 0.0, 1.0),
        x_axis: VectorType = (1.0, 0.0, 0.0),
    ) -> ifcopenshell.entity_instance:
        """
        Create IfcAxis2Placement3D.

        :param position: placement position (Axis).
        :param z_axis: local Z axis direction.
        :param x_axis: local X axis direction (RefDirection).
        :return: IfcAxis2Placement3D
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_axis2_placement_3d",
            _native_vector(position),
            _native_vector(z_axis),
            _native_vector(x_axis),
        )

    def create_axis2_placement_3d_from_matrix(
        self,
        matrix: Union[npt.NDArray[np.float64], None] = None,
    ) -> ifcopenshell.entity_instance:
        """
        Create IfcAxis2Placement3D from numpy matrix.

        :param matrix: 4x4 transformation matrix, defaults to ``np.eye(4)``
        :return: IfcAxis2Placement3D
        """
        if matrix is None:
            matrix = np.eye(4, dtype=float)
        return self.create_axis2_placement_3d(position=matrix[:3, 3], z_axis=matrix[:3, 2], x_axis=matrix[:3, 0])

    def create_axis2_placement_2d(
        self, position: VectorType = (0.0, 0.0), x_direction: Optional[VectorType] = None
    ) -> ifcopenshell.entity_instance:
        """Create IfcAxis2Placement2D.

        :param position: 2D origin of the placement.
        :param x_direction: Direction of the local X axis. If not provided, defaults to
            the global X axis ``(1, 0)``.
        :return: IfcAxis2Placement2D
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_axis2_placement_2d",
            _native_vector(position),
            _native_vector(x_direction if x_direction else ()),
            x_direction is not None,
        )

    def vertex(self, position: VectorType = (0.0, 0.0, 0.0)) -> ifcopenshell.entity_instance:
        """Create a topological vertex

        Commonly used in structural point elements.

        :param position: The 3D coordinate of the vertex
        :return: IfcVertexPoint
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_vertex",
            _native_vector(position),
        )

    def edge(
        self, start: VectorType = (0.0, 0.0, 0.0), end: VectorType = (1.0, 0.0, 0.0)
    ) -> ifcopenshell.entity_instance:
        """Create a topological edge

        :param start: The start coordinates of the vertex.
        :param end: The end coordinates of the vertex.
        :return: IfcEdge
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_edge",
            _native_vector(start),
            _native_vector(end),
        )

    def face(self, points: SequenceOfVectors) -> ifcopenshell.entity_instance:
        """Create a single topological face

        There are many types of faces, but for now we only support planar
        polyloop defined faces with an outer boundary.

        :param points: ordered list of 3d coordinates representing the outer boundary
        :return: IfcFace
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_face",
            _native_points(points),
        )

    def mirror(
        self,
        curve_or_item: Union[ifcopenshell.entity_instance, list[ifcopenshell.entity_instance]],
        mirror_axes: Union[VectorType, SequenceOfVectors] = (1.0, 1.0),
        mirror_point: VectorType = (0.0, 0.0),
        create_copy: bool = False,
        placement_matrix: Optional[np.ndarray] = None,
    ) -> Union[ifcopenshell.entity_instance, list[ifcopenshell.entity_instance]]:
        """Mirror curve/representaiton item/representation.

        :param curve_or_item: A single item to mirror or a sequence of them.
        :param mirror_axes: A vector of values, should have value > 0 for axes where mirror should be applied.
            Example: mirroring `A(1,0)` by axis `(1,0)` will result in `A'(-1,0)`

            Also could be a list of mirrors to apply to `curve_or_item`
            multiple mirror_axes will result in multiple resulting curves
            Example: curve_or_item = [a, b], mirror_axes=[v1, v2], result = [av1, av2, bv1, bv2]
        :param mirror_point: Point relative to which mirror should be applied.
        :param create_copy: Whether to mirror the provided item or it's copy.
        :param placement_matrix: Optional placement matrix to use for polylines.
        :return: Mirrored curve/item/representation or a sequence of them.
        """

        multiple_objects = isinstance(curve_or_item, collections.abc.Iterable)
        curve_or_item = [curve_or_item] if not multiple_objects else curve_or_item
        multiple_transformations = not isinstance(mirror_axes[0], (float, int))
        mirror_axes_data = [mirror_axes] if not multiple_transformations else mirror_axes

        processed_objects: list[ifcopenshell.entity_instance] = []
        for curve_or_item_el in curve_or_item:
            for mirror_axes in mirror_axes_data:
                processed_objects.append(
                    _native_entity(
                        self.file,
                        "ifcopenshell_ifcapi_shape_builder_mirror",
                        curve_or_item_el._handle,
                        _native_vector(mirror_axes),
                        _native_vector(mirror_point),
                        bool(create_copy),
                        _native_vector(
                            []
                            if placement_matrix is None
                            else np.array(placement_matrix, dtype="d")[:3, :3].reshape(9).tolist()
                        ),
                    )
                )

        return processed_objects if (multiple_objects or multiple_transformations) else processed_objects[0]

    def sphere(self, radius: float = 1.0, center: VectorType = (0.0, 0.0, 0.0)) -> ifcopenshell.entity_instance:
        """
        :param radius: radius of the sphere.
        :param center: sphere position.

        :return: IfcSphere
        """

        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_sphere",
            float(radius),
            _native_vector(center),
        )

    def block(
        self,
        position: VectorType = (0.0, 0.0, 0.0),
        x_length: float = 1.0,
        y_length: float = 1.0,
        z_length: float = 1.0,
    ) -> ifcopenshell.entity_instance:
        """
        :param position: the bottom left (min X/Y of the cube).
        :param x_length: the X length, in the +X direction
        :param y_length: the Y length, in the +Y direction
        :param z_length: the Z length, in the +Z direction

        :return: IfcBlock
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_block",
            _native_vector(position),
            float(x_length),
            float(y_length),
            float(z_length),
        )

    def half_space_solid(
        self, plane: ifcopenshell.entity_instance, agreement_flag: bool = False
    ) -> ifcopenshell.entity_instance:
        """
        :param plane: The IfcPlane representing the half space.
        :param agreement_flag: If False (default), the plane normal points toward the **removed** material (the void). The kept region is on the opposite side from the normal.
        :return: IfcHalfSpaceSolid
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_half_space_solid",
            plane._handle,
            bool(agreement_flag),
        )

    def extrude(
        self,
        profile_or_curve: ifcopenshell.entity_instance,
        magnitude: float = 1.0,
        position: VectorType = (0.0, 0.0, 0.0),
        extrusion_vector: VectorType = (0.0, 0.0, 1.0),
        position_z_axis: VectorType = (0.0, 0.0, 1.0),
        position_x_axis: VectorType = (1.0, 0.0, 0.0),
        position_y_axis: Optional[VectorType] = None,
    ) -> ifcopenshell.entity_instance:
        """Extrude profile or curve to get IfcExtrudedAreaSolid.

        REMEMBER when handling custom axes - IFC is using RIGHT handed coordinate system.

        Position and position axes are in world space, extrusion vector in placement space defined by
        position_x_axis/position_y_axis/position_z_axis

        NOTE: changing position also changes the resulting geometry origin.

        :param profile_or_curve: Profile or a curve to extrude (curve will automatically converted to a profile).
        :param extrusion_vector: as defined in coordinate system position_x_axis+position_z_axis
        :param position: as defined in default IFC coordinate system, not in position_x_axis+position_z_axis
        :param position_y_axis: optional, could be used to calculate Z-axis based on Y-axis
        :return: IfcExtrudedAreaSolid
        """

        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_extrude",
            profile_or_curve._handle,
            float(magnitude),
            _native_vector(position),
            _native_vector(extrusion_vector),
            _native_vector(position_z_axis),
            _native_vector(position_x_axis),
            _native_vector(position_y_axis if position_y_axis is not None else ()),
            position_y_axis is not None,
        )

    def create_swept_disk_solid(
        self, path_curve: ifcopenshell.entity_instance, radius: float
    ) -> ifcopenshell.entity_instance:
        """Create an IfcSweptDiskSolid — a circular cross-section swept along a 3D path.

        Useful for modelling round pipes, conduits, and cables.

        :param path_curve: A 3D curve entity defining the centreline path. Must have ``Dim == 3``.
        :param radius: Radius of the circular disk cross-section.
        :return: IfcSweptDiskSolid
        """
        path_curve_dim = _curve_dim(path_curve)
        if path_curve_dim != 3:
            raise Exception(
                f"Path curve for IfcSweptDiskSolid should be 3D to be valid, currently it has {path_curve_dim} dimensions.\n"
                "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcSweptDiskSolid.htm#8.8.3.42.4-Formal-propositions"
            )

        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_swept_disk_solid",
            path_curve._handle,
            float(radius),
        )

    def get_representation(
        self,
        context: ifcopenshell.entity_instance,
        items: Union[ifcopenshell.entity_instance, Sequence[ifcopenshell.entity_instance]],
        representation_type: Optional[str] = None,
    ) -> ifcopenshell.entity_instance:
        """Create IFC representation for the specified context and items.

        **All items must belong to the same geometry category.** IFC prohibits
        mixing incompatible item types in one representation (e.g.
        ``IfcExtrudedAreaSolid`` with ``IfcBlock``, or solids with curves).
        When ``representation_type`` is omitted the type is inferred via
        :func:`ifcopenshell.util.representation.guess_type`; if the items are
        heterogeneous ``guess_type`` returns ``None`` and the representation is
        written with no ``RepresentationType``, which fails IFC validation.
        Avoid mixing swept-solid primitives (``IfcExtrudedAreaSolid``,
        ``IfcRevolvedAreaSolid``) with CSG primitives (``IfcBlock``,
        ``IfcSphere``, etc.) or any other category in a single call.

        :param context: IfcGeometricRepresentationSubContext
        :param items: A single item or list of items, all of the same geometry
            category (e.g. all ``IfcExtrudedAreaSolid``, all ``IfcIndexedPolyCurve``)
        :param representation_type: Explicitly specified RepresentationType.
            If not provided it will be guessed from the items types.
        :return: IfcShapeRepresentation
        """
        if not isinstance(items, collections.abc.Iterable):
            items = [items]

        if not representation_type:
            representation_type = ifcopenshell.util.representation.guess_type(items)

        return self.file.create_entity(
            (
                "IfcTopologyRepresentation"
                if representation_type in ("Vertex", "Edge", "Path", "Face", "Shell")
                else "IfcShapeRepresentation"
            ),
            ContextOfItems=context,
            RepresentationIdentifier=context.ContextIdentifier,
            RepresentationType=representation_type,
            Items=items,
        )

    def deep_copy(self, element: ifcopenshell.entity_instance) -> ifcopenshell.entity_instance:
        """Create a deep copy of an IFC element and all its referenced entities.

        :param element: The IFC entity to copy.
        :return: A new independent copy of the element.
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_deep_copy",
            element._handle,
        )

    # UTILITIES
    def extrude_kwargs(self, axis: Literal["Y", "X", "Z"]) -> dict[str, tuple[float, float, float]]:
        """Shortcut to get kwargs for :meth:`extrude` to extrude along a principal axis.

        Assumes the 2D profile lies in the plane perpendicular to the extrusion axis:
        XZ plane for Y-axis extrusion, YZ plane for X-axis extrusion, XY plane for Z-axis extrusion.

        Extruding along X or Y with other kwargs may violate the IFC ValidExtrusionDirection constraint.

        :param axis: The extrusion axis: ``'X'``, ``'Y'``, or ``'Z'``.
        :return: A dict with keys ``position_x_axis``, ``position_z_axis``, and ``extrusion_vector``
            suitable for passing as ``**kwargs`` to :meth:`extrude`.
        """

        if axis == "Y":
            return {
                "position_x_axis": (1, 0, 0),
                "position_z_axis": (0, -1, 0),
                "extrusion_vector": (0, 0, -1),
            }
        elif axis == "X":
            return {
                "position_x_axis": (0, 1, 0),
                "position_z_axis": (1, 0, 0),
                "extrusion_vector": (0, 0, 1),
            }
        elif axis == "Z":
            return {
                "position_x_axis": (1, 0, 0),
                "position_z_axis": (0, 0, 1),
                "extrusion_vector": (0, 0, 1),
            }

    def rotate_extrusion_kwargs_by_z(
        self, kwargs: dict[str, Any], angle: float, counter_clockwise: bool = False
    ) -> dict[str, VectorType]:
        """Rotate extrusion kwargs around the Z axis.

        A shortcut to rotate the ``position_x_axis`` and ``position_z_axis`` values returned by
        :meth:`extrude_kwargs` around the Z axis before passing them to :meth:`extrude`.

        :param kwargs: A dict with ``position_x_axis`` and ``position_z_axis`` keys,
            as returned by :meth:`extrude_kwargs`. The original dict is not mutated.
        :param angle: Rotation angle, in radians.
        :param counter_clockwise: If True, rotate counter-clockwise. Defaults to clockwise.
        :return: A new dict with ``position_x_axis`` and ``position_z_axis`` rotated around Z.
        """
        rot = np_rotation_matrix(-angle, 3, "Z")
        kwargs = kwargs.copy()  # prevent mutation of original kwargs
        kwargs["position_x_axis"] = rot @ kwargs["position_x_axis"]
        kwargs["position_z_axis"] = rot @ kwargs["position_z_axis"]
        return kwargs

    def get_polyline_coords(self, polyline: ifcopenshell.entity_instance) -> np.ndarray:
        """Extract the coordinate array from a polyline entity.

        :param polyline: An ``IfcIndexedPolyCurve`` or ``IfcPolyline`` entity.
        :return: Numpy array of the polyline's point coordinates.
        """
        coords = _capi.shape_builder_get_polyline_coords(polyline._handle)
        return np.array(coords)

    def set_polyline_coords(self, polyline: ifcopenshell.entity_instance, coords: SequenceOfVectors) -> None:
        """Update the coordinates of a polyline entity in-place.

        :param polyline: An ``IfcIndexedPolyCurve`` or ``IfcPolyline`` entity.
        :param coords: New sequence of point coordinates. Must contain the same number of
            points as the original polyline.
        """
        _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_set_polyline_coords",
            polyline._handle,
            _native_points(coords),
        )

    def get_simple_2dcurve_data(
        self,
        coords: SequenceOfVectors,
        fillets: Sequence[int] = (),
        fillet_radius: Union[float, Sequence[float]] = (),
        closed: bool = True,
        create_ifc_curve: bool = False,
    ) -> tuple[list[VectorType], list[list[int]], Union[ifcopenshell.entity_instance, None]]:
        """
        Creates simple 2D curve from set of 2d coords and list of points with fillets.
        Simple curve means that all fillets are based on 90 degree angle.

        :param coords:           list of 2d coords. Example: ((x0,y0), (x1,y1), (x2, y2))
        :param fillets:          list of points from `coords` to base fillet on. Example: (1,)
        :param fillet_radius:    list of fillet radius for each of corresponding point form `fillets`.
            Example: (5.,) Note: `fillet_radius` could be just 1 float value if it's the same for all fillets.
        :param closed:           boolean whether curve should be closed (whether last point connected to first one).
        :param create_ifc_curve: create IfcIndexedPolyCurve or just return the data.

        :return: (points, segments, ifc_curve) for the created simple curve
            if both points in e are equally far from pt, then v1 is returned.
        """

        def remove_redundant_points(
            points: list[VectorType], segments: list[list[int]]
        ) -> tuple[list[VectorType], list[list[int]]]:
            # prevent mutating
            points = [tuple(p) for p in points]
            segments = segments.copy()

            # find duplicate points, reindex them in segments
            # and mark them to delete later
            points_to_remove: list[int] = []
            prev_point = 0
            for i, p in enumerate(points[1:], 1):
                if p != points[prev_point]:
                    prev_point = i
                    continue

                valid_segments: list[list[int]] = []
                for s in segments:
                    s = [ps if ps != i else prev_point for ps in s]
                    valid_segments.append(s)
                segments = valid_segments
                points_to_remove.append(i)

            # remove duplicate segments
            valid_segments = [segment for segment in segments if len(set(segment)) != 1]
            points = [point for i, point in enumerate(points) if i not in points_to_remove]
            # correct the order in segments
            unique_points = sorted(set(chain(*valid_segments)))
            unique_points_translation = {prev: i for i, prev in enumerate(unique_points)}
            valid_segments = [[unique_points_translation[p] for p in s] for s in valid_segments]

            return points, valid_segments

        # option to use same fillet radius for all fillets
        if isinstance(fillet_radius, (float, int)):
            fillet_radius = [fillet_radius] * len(fillets)

        fillets: dict[int, float] = dict(zip(fillets, fillet_radius))
        segments: list[list[int]] = []
        points: list[VectorType] = []

        for co_i, co in enumerate(coords, 0):
            current_point = len(points)
            if co_i in fillets:
                r = fillets[co_i]
                rsb = r * cos(pi / 4)  # radius shift big
                rss = r - rsb  # radius shift small

                next_co = coords[(co_i + 1) % len(coords)]
                previous_co = coords[co_i - 1]

                # identify fillet type (1 of 4 possible types)
                x_direction = 1 if coords[co_i][0] < previous_co[0] or coords[co_i][0] < next_co[0] else -1
                y_direction = 1 if coords[co_i][1] < previous_co[1] or coords[co_i][1] < next_co[1] else -1

                xshift_point = (co[0] + r * x_direction, co[1])
                middle_point = (co[0] + rss * x_direction, co[1] + rss * y_direction)
                yshift_point = (co[0], co[1] + r * y_direction)

                # identify fillet direction
                if co[1] == previous_co[1]:
                    points.extend((xshift_point, middle_point, yshift_point))
                else:
                    points.extend((yshift_point, middle_point, xshift_point))

                segments.append([current_point - 1, current_point])
                segments.append([current_point, current_point + 1, current_point + 2])
            else:
                points.append(co)
                if co_i != 0:
                    segments.append([current_point - 1, current_point])

        if closed:
            segments.append([len(points) - 1, 0])

        # replace negative index
        if segments[0][0] == -1:
            segments[0][0] = len(points) - 1

        # sometime fillet points could match previous or next points in line
        # I remove them at the end to avoid making fillet algorithm even less readable
        points, segments = remove_redundant_points(points, segments)
        ifc_curve = None
        if create_ifc_curve:
            ifc_curve = _native_entity(
                self.file,
                "ifcopenshell_ifcapi_shape_builder_indexed_polycurve_2d",
                _native_points(points),
                _native_faces([[i + 1 for i in segment] for segment in segments]),
            )
        return (points, segments, ifc_curve)

    def create_z_profile_lips_curve(
        self,
        FirstFlangeWidth: float,
        SecondFlangeWidth: float,
        Depth: float,
        Girth: float,
        WallThickness: float,
        FilletRadius: float,
    ) -> ifcopenshell.entity_instance:
        """Create a Z-profile (cold-formed steel section) outline curve with lips and fillets.

        All dimensions are in the IFC project's length units.

        :param FirstFlangeWidth: Width of the first (top) flange, measured from the web centreline.
        :param SecondFlangeWidth: Width of the second (bottom) flange, measured from the web centreline.
        :param Depth: Total depth of the section (web height).
        :param Girth: Length of the return lips on each flange.
        :param WallThickness: Uniform material thickness.
        :param FilletRadius: Inner bend radius at each corner.
        :return: IfcIndexedPolyCurve representing the closed Z-profile outline.
        """
        x1 = FirstFlangeWidth
        x2 = SecondFlangeWidth
        y = Depth / 2
        g = Girth
        t = WallThickness
        r = FilletRadius

        # fmt: off
        coords = (
            (-t/2,   y),
            (x2,     y),
            (x2,     y-g),
            (x2-t,   y-g),
            (x2-t,   y-t),
            (t/2,    y-t),
            (t/2,   -y),
            (-x1,   -y),
            (-x1,   -y+g),
            (-x1+t, -y+g),
            (-x1+t, -y+t),
            (-t/2,  -y+t)
        )

        # option for no additional thickness in outer radius:
        # points, segments, ifc_curve = create_curve_from_coords(
        #   coords, fillets = (0, 1, 4, 5, 6, 7, 10, 11), fillet_radius=r, closed=True, ifc_file=ifc_file
        # )

        points, segments, ifc_curve = self.get_simple_2dcurve_data(
            coords,
            fillets =     (0,   1,   4, 5, 6,   7,   10, 11),
            fillet_radius=(r+t, r+t, r, r, r+t, r+t, r, r),
            closed=True, create_ifc_curve=True)
        # fmt: on
        assert ifc_curve

        return ifc_curve

    def create_transition_arc_ifc(
        self, width: float, height: float, create_ifc_curve: bool = False
    ) -> tuple[SequenceOfVectors, list[list[int]], Union[ifcopenshell.entity_instance, None]]:
        """Create an arc fitting inside a rectangle of the given width and height.

        If a single arc cannot span the full width, the longest possible radius is used and
        a straight segment is inserted in the middle.

        :param width: Width of the bounding rectangle.
        :param height: Height of the bounding rectangle (also the maximum arc radius).
        :param create_ifc_curve: If True, also create and return an ``IfcIndexedPolyCurve``.
            If False, only return the raw point and segment data.
        :return: A tuple ``(points, segments, ifc_curve)`` where ``ifc_curve`` is an
            ``IfcIndexedPolyCurve`` when ``create_ifc_curve=True``, otherwise ``None``.
        """
        fillet_size = (width / 2) / height
        if fillet_size <= 1:
            fillet_radius = height * fillet_size
            curve_coords = [
                (0.0, 0.0),
                (0.0, height),
                (width * 0.5, height),
                (width, height),
                (width, 0.0),
            ]
            fillets = (1, 3)
        else:
            fillet_radius = height
            curve_coords = [
                (0.0, 0.0),
                (0.0, height),
                (fillet_radius, height),
                (width - fillet_radius, height),
                (width, height),
                (width, 0.0),
            ]
            fillets = (1, 4)
        points, segments, transition_arc = self.get_simple_2dcurve_data(
            curve_coords, fillets, fillet_radius, closed=False, create_ifc_curve=create_ifc_curve
        )
        return points, segments, transition_arc

    def mesh(self, points: SequenceOfVectors, faces: Sequence[Sequence[int]]) -> ifcopenshell.entity_instance:
        """Create a tessellated mesh from points and face indices.

        Delegates to :meth:`faceted_brep` for IFC2X3, or :meth:`polygonal_face_set` for IFC4 and later.

        :param points: List of 3D coordinates.
        :param faces: List of faces, each face a sequence of zero-based point indices.
        :return: IfcFacetedBrep (IFC2X3) or IfcPolygonalFaceSet (IFC4+).
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_mesh",
            _native_points(points),
            _native_faces(faces),
        )

    def faceted_brep(self, points: SequenceOfVectors, faces: Sequence[Sequence[int]]) -> ifcopenshell.entity_instance:
        """Generate an IfcFacetedBrep with a closed shell

        Note that :func:`polygonal_face_set` is recommended in IFC4.

        :param points: list of 3d coordinates
        :param faces: list of faces consisted of point indices (points indices starting from 0)
        :return: IfcFacetedBrep
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_faceted_brep",
            _native_points(points),
            _native_faces(faces),
        )

    def triangulated_face_set(
        self, points: SequenceOfVectors, faces: Sequence[Sequence[int]]
    ) -> ifcopenshell.entity_instance:
        """
        Generate an IfcTriangulatedFaceSet

        Note that this is not available in IFC2X3.

        :param points: list of 3d coordinates
        :param faces: list of triangles consisted of point indices (points indices starting from 0)
        :return: IfcTriangulatedFaceSet
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_triangulated_face_set",
            _native_points(points),
            _native_faces(faces),
        )

    def polygonal_face_set(
        self, points: SequenceOfVectors, faces: Sequence[Union[Sequence[int], Sequence[Sequence[int]]]]
    ) -> ifcopenshell.entity_instance:
        """
        Generate an IfcPolygonalFaceSet

        Note that this is not available in IFC2X3.

        :param points: list of 3d coordinates
        :param faces: list of faces consisted of point indices (points indices starting from 0)
                      in case of multiple sequences per face, the subsequent ones are inner voids
        :return: IfcPolygonalFaceSet
        """
        return _native_entity(
            self.file,
            "ifcopenshell_ifcapi_shape_builder_polygonal_face_set",
            _native_points(points),
            _native_polygonal_faces(faces),
        )

    def extrude_face_set(
        self,
        points: SequenceOfVectors,
        magnitude: float,
        extrusion_vector: VectorType = (0, 0, 1),
        offset: Optional[VectorType] = None,
        start_cap: bool = True,
        end_cap: bool = True,
    ) -> ifcopenshell.entity_instance:
        """
        Method to extrude by creating face sets rather than creating IfcExtrudedAreaSolid.

        Useful if your representation is already using face sets and you need to avoid using SweptSolid
        to assure CorrectItemsForType.

        :param points: list of points, assuming they form consecutive closed polyline.
        :param magnitude: extrusion magnitude
        :param extrusion_vector: extrusion direction.
        :param offset: offset from the points
        :param start_cap: if True, create start cap.
        :param end_cap: if True, create end cap.
        :return: IfcPolygonalFaceSet
        """

        # prevent mutating arguments, deepcopy doesn't work
        start_points = np.array(points)
        if offset is not None and offset.any():
            start_points += offset
        extrusion_offset = np.multiply(extrusion_vector, magnitude)
        end_points = start_points + extrusion_offset

        all_points = np.vstack((start_points, end_points))
        faces = []
        n_verts = len(start_points)
        last_vert_i = n_verts - 1
        for i in range(last_vert_i):
            face = (i, i + 1, n_verts + i + 1, n_verts + i)
            faces.append(face)
        faces.append((last_vert_i, 0, n_verts + 0, n_verts + last_vert_i))  # close the loop

        if end_cap:
            faces.append(tuple(range(n_verts, n_verts * 2)))
        if start_cap:
            faces.append(tuple(reversed(range(n_verts))))

        face_set = self.polygonal_face_set(all_points, faces)
        return face_set

    # TODO: move MEP to separate shape builder sub module
    def mep_transition_shape(
        self,
        start_segment: ifcopenshell.entity_instance,
        end_segment: ifcopenshell.entity_instance,
        start_length: float,
        end_length: float,
        angle: float = 30.0,
        profile_offset: VectorType = (0.0, 0.0),
    ) -> Union[tuple[ifcopenshell.entity_instance, dict[str, Any]], tuple[None, None]]:
        """Generate a MEP transition shape for the provided segments.

        :param start_segment: Starting segment.
        :param end_segment: Ending segment.
        :param start_length: Start transition length.
        :param end_length: End transition length.
        :param angle: Transition angle, in degrees.
            Good default values from angle = 30/60 deg
            30 degree angle will result in 75 degrees on the transition (= 90 - α/2) - https://i.imgur.com/tcoYDWu.png
        :param profile_offset: 2D vector for profile offset.
        :return: A tuple of Model/Body/MODEL_VIEW IfcRepresentation and dictionary of transition shape data.
            Or (None, None) if there was an error in the process.
        """
        value = _capi.shape_builder_mep_transition_shape(
            self.file._handle,
            start_segment._handle,
            end_segment._handle,
            float(start_length),
            float(end_length),
            float(angle),
            _native_vector(profile_offset),
        )
        if not _struct_field(value, "has_result", 1):
            return None, None
        representation = _native_entity_from_struct_handle(
            self.file, _struct_field(value, "representation", 0), "shape_builder_mep_transition_shape"
        )
        return representation, {
            "start_length": _struct_field(value, "start_length", 2),
            "end_length": _struct_field(value, "end_length", 3),
            "angle": _struct_field(value, "angle", 4),
            "profile_offset": _struct_field(value, "profile_offset", 5),
            "transition_length": _struct_field(value, "transition_length", 6),
            "full_transition_length": _struct_field(value, "full_transition_length", 7),
        }

    # TODO: move to separate shape_builder method
    # so we could check transition length without creating representation
    def mep_transition_length(
        self,
        start_half_dim: np.ndarray,
        end_half_dim: np.ndarray,
        angle: float,
        profile_offset: VectorType = (0.0, 0.0),
        verbose: bool = True,
    ) -> Optional[float]:
        """Get the transition length for two profile half-dimensions, an angle, and an XY offset.

        Unlike :meth:`mep_transition_calculate`, this method checks that the resulting length
        satisfies the angle constraint from both the start and end profile perspectives.

        :param start_half_dim: Half-dimensions of the start profile as a 3-element array
            ``[half_x, half_y, depth]``. For circular profiles ``half_x == half_y == radius``.
        :param end_half_dim: Half-dimensions of the end profile in the same format.
        :param angle: Maximum allowed transition angle, in degrees.
        :param profile_offset: 2D XY offset between the centrelines of the start and end profiles.
        :param verbose: If True, print diagnostic values during calculation.
        :return: Transition length in project length units, or ``None`` if no valid length exists
            for the given angle and offset.
        """
        result = _capi.shape_builder_mep_transition_length(
            _native_vector(start_half_dim),
            _native_vector(end_half_dim),
            float(angle),
            _native_vector(profile_offset),
        )
        return None if np.isnan(result) else result

    def mep_transition_calculate(
        self,
        start_half_dim: np.ndarray,
        end_half_dim: np.ndarray,
        offset: np.ndarray,
        diff: Optional[np.ndarray] = None,
        end_profile: bool = False,
        length: Optional[float] = None,
        angle: Optional[float] = None,
        verbose: bool = True,
    ) -> Union[float, None]:
        """Calculate MEP transition length from angle, or transition angle from length.

        Low-level calculation kernel used by :meth:`mep_transition_length`. Provide either
        ``angle`` or ``length`` (not both); the other value is computed and returned.

        :param start_half_dim: Half-dimensions of the start profile ``[half_x, half_y, depth]``.
        :param end_half_dim: Half-dimensions of the end profile ``[half_x, half_y, depth]``.
        :param offset: 2D XY offset between profile centrelines.
        :param diff: Pre-computed absolute difference of start and end half-dimensions (XY only).
            Computed from ``start_half_dim`` and ``end_half_dim`` if not provided.
        :param end_profile: If True, swap X and Y axes to compute from the end-profile perspective.
        :param length: Known transition length. If provided, the corresponding angle is returned.
        :param angle: Known transition angle, in degrees. If provided, the corresponding length is returned.
        :param verbose: If True, print diagnostic values during calculation.
        :return: Transition length (if ``angle`` was given) or transition angle in degrees
            (if ``length`` was given), or ``None`` if the geometry is not feasible.
        """
        result = _capi.shape_builder_mep_transition_calculate(
            _native_vector(start_half_dim),
            _native_vector(end_half_dim),
            _native_vector(offset),
            _native_vector(diff if diff is not None else ()),
            diff is not None,
            bool(end_profile),
            0.0 if length is None else float(length),
            length is not None,
            0.0 if angle is None else float(angle),
            angle is not None,
        )
        return None if np.isnan(result) else result

    def mep_bend_shape(
        self,
        segment: ifcopenshell.entity_instance,
        start_length: float,
        end_length: float,
        angle: float,
        radius: float,
        bend_vector: VectorType,
        flip_z_axis: bool,
    ) -> tuple[ifcopenshell.entity_instance, dict[str, Any]]:
        """
        Generate a MEP bend shape for the provided segments.

        :param segment: IfcFlowSegment for a bend.
            Note that for a bend start and end segments types should match.
        :param angle: bend angle, in radians
        :param radius: bend radius
        :param bend_vector: offset between start and end segments in local space of start segment
            used mainly to determine the second bend axis and it's direction (positive or negative),
            the actual magnitude of the vector is not important (though near zero values will be ignored).
        :param flip_z_axis: since we cannot determine z axis direction from the profile offset,
            there is an option to flip it if bend is going by start segment Z- axis.
        :return: tuple of Model/Body/MODEL_VIEW IfcRepresentation and dictionary of transition shape data
        """
        value = _capi.shape_builder_mep_bend_shape(
            self.file._handle,
            segment._handle,
            float(start_length),
            float(end_length),
            float(angle),
            float(radius),
            _native_vector(bend_vector),
            bool(flip_z_axis),
        )
        representation = _native_entity_from_struct_handle(
            self.file, _struct_field(value, "representation", 0), "shape_builder_mep_bend_shape"
        )
        return representation, {
            "start_length": _struct_field(value, "start_length", 1),
            "end_length": _struct_field(value, "end_length", 2),
            "radius": _struct_field(value, "radius", 3),
            "angle": _struct_field(value, "angle", 4),
            "lateral_axis": _struct_field(value, "lateral_axis", 5),
            "lateral_sign": _struct_field(value, "lateral_sign", 6),
            "z_axis_sign": _struct_field(value, "z_axis_sign", 7),
            "main_profile_dimension": _struct_field(value, "main_profile_dimension", 8),
        }
