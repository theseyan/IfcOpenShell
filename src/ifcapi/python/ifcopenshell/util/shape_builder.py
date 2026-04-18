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
from math import atan, cos, degrees, pi, radians, sin, sqrt, tan
from typing import TYPE_CHECKING, Any, Literal, Optional, Union

import numpy as np
import numpy.typing as npt

import ifcopenshell
import ifcopenshell.util.element
import ifcopenshell.util.placement
import ifcopenshell.util.representation
import ifcopenshell.util.unit

PRECISION = 1.0e-5


if TYPE_CHECKING:
    from mathutils import Vector  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]

    VectorType = Union[Sequence[float], Vector, np.ndarray]
else:
    VectorType = Any

SequenceOfVectors = Union[Sequence[VectorType], np.ndarray]


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
        axis = axis / np.linalg.norm(axis)
        K = np.array([[0, -axis[2], axis[1]], [axis[2], 0, -axis[0]], [-axis[1], axis[0], 0]])
        matrix = cos_theta * np.eye(3) + (1 - cos_theta) * np.outer(axis, axis) + sin_theta * K
    if size == 4:
        return np_to_4x4(matrix)
    return matrix


def np_matrix_to_euler(matrix: np.ndarray) -> tuple[float, float, float]:
    """Convert a rotation matrix to Euler angles.

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
    d1 = np.subtract(v2, v1)
    d2 = np.subtract(v4, v3)

    cross_d1_d2 = np.cross(d1, d2)
    cross_d1_d2_norm: float = np.linalg.norm(cross_d1_d2)

    if is_x(cross_d1_d2_norm, 0):
        raise ValueError("Lines are parallel and do not intersect uniquely.")

    r = np.subtract(v3, v1)
    t = np.dot(np.cross(r, d2), cross_d1_d2) / (cross_d1_d2_norm**2)
    u = np.dot(np.cross(r, d1), cross_d1_d2) / (cross_d1_d2_norm**2)

    point_on_line1 = v1 + t * d1
    point_on_line2 = v3 + u * d2
    return point_on_line1, point_on_line2


def intersect_x_axis_2d(p1: VectorType, p2: VectorType, y=0) -> Optional[float]:
    """Intersect a line defined by 2 points to a horizontal line defined by y

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
        :param arc_points: Indices of the middle points for arcs.
        :return: IfcIndexedPolyCurve
        """

        if arc_points and self.file.schema == "IFC2X3":
            raise Exception("Arcs are not supported for IFC2X3.")

        points: np.ndarray
        points = np.array(points)
        if position_offset is not None:
            points = points + position_offset

        if self.file.schema == "IFC2X3":
            ifc_points = [self.file.create_entity("IfcCartesianPoint", p) for p in points.tolist()]
            if closed:
                ifc_points.append(ifc_points[0])
            ifc_curve = self.file.createIfcPolyline(Points=ifc_points)
            return ifc_curve

        dimensions = len(points[0])
        if dimensions == 2:
            ifc_points = self.file.create_entity("IfcCartesianPointList2D", points.tolist())
        elif dimensions == 3:
            ifc_points = self.file.create_entity("IfcCartesianPointList3D", points.tolist())
        else:
            raise Exception(f"Point has unexpected number of dimensions - {dimensions}.")

        if not closed and not arc_points:
            ifc_curve = self.file.createIfcIndexedPolyCurve(Points=ifc_points)
            return ifc_curve

        segments = []
        cur_i = 0
        closed_by_arc = False
        while cur_i < len(points) - 1:
            cur_i_ifc = cur_i + 1
            if cur_i + 1 in arc_points:
                if cur_i_ifc + 1 < len(points):
                    segments.append((cur_i_ifc, cur_i_ifc + 1, cur_i_ifc + 2))
                else:
                    segments.append((cur_i_ifc, cur_i_ifc + 1, 1))
                    closed_by_arc = True
                cur_i += 2
            else:
                segments.append((cur_i_ifc, cur_i_ifc + 1))
                cur_i += 1

        if closed and not closed_by_arc:
            segments.append((len(points), 1))

        ifc_segments = []
        current_line_segment = []
        last_segment = len(segments) - 1
        for seg_i, segment in enumerate(segments):
            if len(segment) == 2:
                current_line_segment += segment if not current_line_segment else segment[1:]

            if current_line_segment and (len(segment) == 3 or seg_i == last_segment):
                ifc_segments.append(self.file.createIfcLineIndex(current_line_segment))
                current_line_segment = []

            if len(segment) == 3:
                ifc_segments.append(self.file.createIfcArcIndex(segment))

        ifc_curve = self.file.createIfcIndexedPolyCurve(Points=ifc_points, Segments=ifc_segments)
        return ifc_curve

    @staticmethod
    def get_rectangle_coords(size: VectorType = (1.0, 1.0), position: Optional[VectorType] = None) -> np.ndarray:
        """
        Get rectangle coords arranged as below:

        ::

            3 2
            0 1

        :param size: rectangle size, could be either 2d or 3d.
        :param position: rectangle position.
        :return: list of rectangle coords
        """
        size_np = np.array(size)

        if position is None:
            dimensions = len(size_np)
            points = np.full((4, dimensions), 0.0)
        else:
            points = np.tile(position, (4, 1))

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
        :return: IfcIndexedPolyCurve
        """
        return self.polyline(self.get_rectangle_coords(size, position), closed=True)

    def circle(self, center: VectorType = (0.0, 0.0), radius: float = 1.0) -> ifcopenshell.entity_instance:
        """
        :param center: circle 2D position
        :param radius: radius of the circle
        :return: IfcCircle
        """
        ifc_center = self.create_axis2_placement_2d(center)
        ifc_curve = self.file.create_entity("IfcCircle", ifc_center, radius)
        return ifc_curve

    def plane(
        self, location: VectorType = (0.0, 0.0, 0.0), normal: VectorType = (0.0, 0.0, 1.0)
    ) -> ifcopenshell.entity_instance:
        """
        Create IfcPlane.

        :param location: plane position.
        :param normal: plane normal direction.
        :return: IfcPlane
        """

        if np.allclose(np.round(normal, 2), (0.0, 0.0, 1.0)):
            arbitrary_vector = (0.0, 1.0, 0.0)
        else:
            arbitrary_vector = (0.0, 0.0, 1.0)
        x_axis = np_normalized(np.cross(normal, arbitrary_vector))
        axis_placement = self.create_axis2_placement_3d(location, normal, x_axis)
        return self.file.createIfcPlane(axis_placement)

    def curve_between_two_points(self, points: tuple[VectorType, VectorType]) -> ifcopenshell.entity_instance:
        """Simple circle based curve between two points

        :param points: tuple of 2 points.
        :return: IfcIndexePolyCurve
        """
        diff = np.subtract(points[1], points[0])
        max_diff_i = np.argmax(np.abs(diff))
        diff_sign = np.zeros_like(diff)
        diff_sign[max_diff_i] = np.sign(diff[max_diff_i])

        diff = (0.01, 0.01) * diff_sign
        middle_point = points[0] + diff

        points: list[VectorType]
        points = [points[0], middle_point, points[1]]
        points = [ifc_safe_vector_type(p) for p in points]
        seg = self.file.createIfcArcIndex((1, 2, 3))
        ifc_points = self.file.createIfcCartesianPointList2D(points)
        curve = self.file.createIfcIndexedPolyCurve(Points=ifc_points, Segments=[seg])
        return curve

    def get_trim_points_from_mask(
        self,
        x_axis_radius: float,
        y_axis_radius: float,
        trim_points_mask: Sequence[int],
        position_offset: Optional[VectorType] = None,
    ) -> np.ndarray:
        """Get cardinal-point coordinates of an ellipse by index mask.

        :param x_axis_radius: Radius along the X axis.
        :param y_axis_radius: Radius along the Y axis.
        :param trim_points_mask: Sequence of cardinal-point indices (0-3) to select.
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

        :param x_axis_radius: Semi-axis length along the local X axis.
        :param y_axis_radius: Semi-axis length along the local Y axis.
        :param position: 2D centre of the ellipse.
        :param trim_points: Explicit pair of 2D trim points.
        :param ref_x_direction: Direction of the local X axis.
        :param trim_points_mask: Pair of cardinal-point indices (0-3).
        :return: IfcEllipse (untrimmed) or IfcTrimmedCurve (trimmed).
        """
        ifc_position = self.create_axis2_placement_2d(position, ref_x_direction)
        ifc_ellipse = self.file.createIfcEllipse(
            Position=ifc_position, SemiAxis1=x_axis_radius, SemiAxis2=y_axis_radius
        )

        if not trim_points:
            if not trim_points_mask:
                return ifc_ellipse
            trim_points = self.get_trim_points_from_mask(
                x_axis_radius, y_axis_radius, trim_points_mask, position_offset=position
            )

        trim1 = [self.file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(trim_points[0]))]
        trim2 = [self.file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(trim_points[1]))]

        trim_ellipse = self.file.createIfcTrimmedCurve(
            BasisCurve=ifc_ellipse, Trim1=trim1, Trim2=trim2, SenseAgreement=True, MasterRepresentation="CARTESIAN"
        )
        return trim_ellipse

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
        if outer_curve.Dim != 2:
            raise Exception(
                f"Outer curve for IfcArbitraryClosedProfileDef/IfcIfcArbitraryProfileDefWithVoid should be 2D to be valid, currently it has {outer_curve.Dim} dimensions.\n"
                "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcArbitraryClosedProfileDef.htm#8.15.3.1.4-Formal-propositions"
            )

        kwargs = {
            "ProfileName": name,
            "ProfileType": profile_type,
            "OuterCurve": outer_curve,
        }

        if inner_curves:
            if not isinstance(inner_curves, collections.abc.Iterable):
                inner_curves = [inner_curves]
                if any(curve.Dim != 2 for curve in inner_curves):
                    raise Exception(
                        "WARNING. InnerCurve for IfcIfcArbitraryProfileDefWithVoid sould be 2D to be valid, "
                        "currently on one of the inner curves is using different amount of dimensions.\n"
                        "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcArbitraryClosedProfileDef.htm#8.15.3.1.4-Formal-propositions"
                    )

            profile = self.file.create_entity("IfcArbitraryProfileDefWithVoids", InnerCurves=inner_curves, **kwargs)
        else:
            profile = self.file.create_entity("IfcArbitraryClosedProfileDef", **kwargs)
        return profile

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
            if create_copy:
                c = ifcopenshell.util.element.copy_deep(self.file, c)

            if c.is_a() in ("IfcIndexedPolyCurve", "IfcPolyline"):
                coords = self.get_polyline_coords(c)
                coords += translation
                self.set_polyline_coords(c, coords)

            elif c.is_a("IfcCircle") or c.is_a("IfcExtrudedAreaSolid") or c.is_a("IfcEllipse"):
                base_position = np.array(c.Position.Location.Coordinates)
                c.Position.Location.Coordinates = ifc_safe_vector_type(base_position + translation)

            elif c.is_a("IfcTessellatedFaceSet"):
                c.Coordinates.CoordList = ifc_safe_vector_type(np.array(c.Coordinates.CoordList) + translation)

            elif c.is_a("IfcShapeRepresentation"):
                for item in c.Items:
                    self.translate(item, translation)

            elif c.is_a("IfcTrimmedCurve"):
                base_position = np.array(c.Trim1[0].Coordinates)
                c.Trim1[0].Coordinates = ifc_safe_vector_type(base_position + translation)

                base_position = np.array(c.Trim2[0].Coordinates)
                c.Trim2[0].Coordinates = ifc_safe_vector_type(base_position + translation)

                self.translate(c.BasisCurve, translation)

            else:
                raise Exception(f"{c} is not supported for translate() method.")

            processed_objects.append(c)

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
        :param angle: Rotation angle, in degrees.
        :param pivot_point: The point to rotate around.
        :param counter_clockwise: If True, rotate counter-clockwise.
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
            if create_copy:
                c = ifcopenshell.util.element.copy_deep(self.file, c)

            if c.is_a() in ("IfcIndexedPolyCurve", "IfcPolyline"):
                original_coords = self.get_polyline_coords(c)
                coords = [self.rotate_2d_point(co, angle, pivot_point, counter_clockwise) for co in original_coords]
                self.set_polyline_coords(c, coords)

            elif c.is_a("IfcCircle"):
                base_position = c.Position.Location.Coordinates
                new_position = self.rotate_2d_point(base_position, angle, pivot_point, counter_clockwise)
                c.Position.Location.Coordinates = ifc_safe_vector_type(new_position)

            elif c.is_a("IfcExtrudedAreaSolid"):
                base_position = c.Position.Location.Coordinates
                new_position = self.rotate_2d_point(base_position[:2], angle, pivot_point, counter_clockwise)
                new_position = np_to_3d(new_position)
                new_position[2] = base_position[2]
                c.Position.Location.Coordinates = ifc_safe_vector_type(new_position)

                self.rotate(c.SweptArea.OuterCurve, angle, pivot_point, counter_clockwise)

            else:
                raise Exception(f"{c} is not supported for rotate() method.")

            processed_objects.append(c)

        return processed_objects if multiple_objects else processed_objects[0]

    def mirror_2d_point(
        self,
        point_2d: VectorType,
        mirror_axes: VectorType = (1.0, 1.0),
        mirror_point: VectorType = (0.0, 0.0),
    ) -> np.ndarray:
        """Mirror a single 2D point across the specified axes.

        :param point_2d: The 2D point to mirror.
        :param mirror_axes: Indicates which axes to mirror across.
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
        return self.file.create_entity(
            "IfcAxis2Placement3D",
            self.file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(position)),
            Axis=self.file.create_entity("IfcDirection", ifc_safe_vector_type(z_axis)),
            RefDirection=self.file.create_entity("IfcDirection", ifc_safe_vector_type(x_axis)),
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
        :param x_direction: Direction of the local X axis.
        :return: IfcAxis2Placement2D
        """
        ref_direction = (
            self.file.create_entity("IfcDirection", ifc_safe_vector_type(x_direction)) if x_direction else None
        )
        return self.file.create_entity(
            "IfcAxis2Placement2D",
            Location=self.file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(position)),
            RefDirection=ref_direction,
        )

    def vertex(self, position: VectorType = (0.0, 0.0, 0.0)) -> ifcopenshell.entity_instance:
        """Create a topological vertex

        :param position: The 3D coordinate of the vertex
        :return: IfcVertexPoint
        """
        return self.file.create_entity(
            "IfcVertexPoint", self.file.create_entity("IfcCartesianPoint", ifc_safe_vector_type(position))
        )

    def edge(
        self, start: VectorType = (0.0, 0.0, 0.0), end: VectorType = (1.0, 0.0, 0.0)
    ) -> ifcopenshell.entity_instance:
        """Create a topological edge

        :param start: The start coordinates of the vertex.
        :param end: The end coordinates of the vertex.
        :return: IfcEdge
        """
        return self.file.create_entity("IfcEdge", self.vertex(start), self.vertex(end))

    def face(self, points: SequenceOfVectors) -> ifcopenshell.entity_instance:
        """Create a single topological face

        :param points: ordered list of 3d coordinates representing the outer boundary
        :return: IfcFace
        """
        verts = [self.file.createIfcCartesianPoint(p) for p in ifc_safe_vector_type(points)]
        return self.file.createIfcFace([self.file.createIfcFaceOuterBound(self.file.createIfcPolyLoop(verts), True)])

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
        :param mirror_point: Point relative to which mirror should be applied.
        :param create_copy: Whether to mirror the provided item or it's copy.
        :param placement_matrix: Optional placement matrix to use for polylines.
        :return: Mirrored curve/item/representation or a sequence of them.
        """

        np_XY = slice(2)
        np_X, np_Y, np_Z = 0, 1, 2

        multiple_objects = isinstance(curve_or_item, collections.abc.Iterable)
        curve_or_item = [curve_or_item] if not multiple_objects else curve_or_item
        multiple_transformations = not isinstance(mirror_axes[0], (float, int))
        mirror_axes_data = [mirror_axes] if not multiple_transformations else mirror_axes

        processed_objects: list[ifcopenshell.entity_instance] = []
        for curve_or_item_el in curve_or_item:
            for mirror_axes in mirror_axes_data:
                c = (
                    ifcopenshell.util.element.copy_deep(self.file, curve_or_item_el)
                    if create_copy
                    else curve_or_item_el
                )

                if c.is_a() in ("IfcIndexedPolyCurve", "IfcPolyline"):
                    original_coords = self.get_polyline_coords(c)
                    inverted_placement_matrix = (
                        np.linalg.inv(placement_matrix) if placement_matrix is not None else None
                    )
                    coords = []
                    for co in original_coords:
                        co_base = co.copy()
                        if placement_matrix is not None:
                            co_base = placement_matrix @ np_to_3d(co_base)
                            co = self.mirror_2d_point(co_base[np_XY], mirror_axes, mirror_point)
                            co = np_to_3d(co, z=co_base[2])
                            co = (inverted_placement_matrix @ co)[np_XY]
                        else:
                            co = self.mirror_2d_point(co_base, mirror_axes, mirror_point)

                        coords.append(co)

                    self.set_polyline_coords(c, coords)

                elif c.is_a("IfcCircle") or c.is_a("IfcEllipse"):
                    base_position = c.Position.Location.Coordinates
                    new_position = self.mirror_2d_point(base_position, mirror_axes, mirror_point)
                    c.Position.Location.Coordinates = ifc_safe_vector_type(new_position)

                elif c.is_a("IfcExtrudedAreaSolid"):
                    placement_matrix_ = ifcopenshell.util.placement.get_axis2placement(c.Position)[:3, :3]
                    base_position = c.Position.Location.Coordinates
                    new_position = self.mirror_2d_point(base_position[np_XY], mirror_axes, mirror_point)
                    new_position = np_to_3d(new_position, base_position[np_Z])
                    c.Position.Location.Coordinates = ifc_safe_vector_type(new_position)

                    self.translate(c.SweptArea.OuterCurve, base_position[np_XY])
                    self.mirror(c.SweptArea.OuterCurve, mirror_axes, mirror_point, placement_matrix=placement_matrix_)
                    self.translate(c.SweptArea.OuterCurve, -new_position[np_XY])

                    if hasattr(c.SweptArea, "InnerCurves"):
                        for inner_curve in c.SweptArea.InnerCurves:
                            self.translate(inner_curve, base_position[np_XY])
                            self.mirror(inner_curve, mirror_axes, mirror_point, placement_matrix=placement_matrix_)
                            self.translate(inner_curve, -new_position[np_XY])

                    base_extruded_direction = c.ExtrudedDirection.DirectionRatios
                    extruded_direction = placement_matrix_ @ base_extruded_direction

                    new_direction = self.mirror_2d_point(
                        extruded_direction[np_XY], mirror_axes, mirror_point=(0.0, 0.0)
                    )
                    new_direction = np_to_3d(new_direction, extruded_direction[np_Z])

                    new_direction = np.linalg.inv(placement_matrix_) @ (new_direction)
                    c.ExtrudedDirection.DirectionRatios = ifc_safe_vector_type(new_direction)

                elif c.is_a("IfcTrimmedCurve"):
                    trim_coords = [c.Trim1[0].Coordinates, c.Trim2[0].Coordinates]
                    trim_coords = [
                        self.mirror_2d_point(base_position, mirror_axes, mirror_point) for base_position in trim_coords
                    ]

                    if 0 in mirror_axes:
                        trim_coords = [trim_coords[1], trim_coords[0]]

                    trim_coords = ifc_safe_vector_type(np.array(trim_coords))
                    c.Trim1[0].Coordinates, c.Trim2[0].Coordinates = trim_coords

                    self.mirror(c.BasisCurve, mirror_axes, mirror_point)
                else:
                    raise Exception(f"{c} is not supported for mirror() method.")

                processed_objects.append(c)

        return processed_objects if (multiple_objects or multiple_transformations) else processed_objects[0]

    def sphere(self, radius: float = 1.0, center: VectorType = (0.0, 0.0, 0.0)) -> ifcopenshell.entity_instance:
        """
        :param radius: radius of the sphere.
        :param center: sphere position.
        :return: IfcSphere
        """

        ifc_position = self.create_axis2_placement_3d(position=center)
        return self.file.createIfcSphere(Radius=radius, Position=ifc_position)

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
        return self.file.createIfcBlock(self.create_axis2_placement_3d(position), x_length, y_length, z_length)

    def half_space_solid(
        self, plane: ifcopenshell.entity_instance, agreement_flag: bool = False
    ) -> ifcopenshell.entity_instance:
        """
        :param plane: The IfcPlane representing the half space.
        :param agreement_flag: If False (default), the plane normal points toward the removed material.
        :return: IfcHalfSpaceSolid
        """
        return self.file.createIfcHalfSpaceSolid(plane, AgreementFlag=agreement_flag)

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

        :param profile_or_curve: Profile or a curve to extrude.
        :param extrusion_vector: as defined in coordinate system position_x_axis+position_z_axis
        :param position: as defined in default IFC coordinate system
        :param position_y_axis: optional, could be used to calculate Z-axis based on Y-axis
        :return: IfcExtrudedAreaSolid
        """

        if not magnitude:
            raise Exception(
                "Extrusion magnitude must be greater than 0 to be valid.\n"
                "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcPositiveLengthMeasure.htm#8.11.2.71.3-Formal-representation"
            )

        if not profile_or_curve.is_a("IfcProfileDef"):
            profile_or_curve = self.profile(profile_or_curve)

        if position_y_axis:
            position_z_axis = np.cross(position_x_axis, position_y_axis)

        ifc_position = self.create_axis2_placement_3d(position, position_z_axis, position_x_axis)
        ifc_direction = self.file.create_entity("IfcDirection", ifc_safe_vector_type(extrusion_vector))
        extruded_area = self.file.createIfcExtrudedAreaSolid(
            SweptArea=profile_or_curve, Position=ifc_position, ExtrudedDirection=ifc_direction, Depth=magnitude
        )
        return extruded_area

    def create_swept_disk_solid(
        self, path_curve: ifcopenshell.entity_instance, radius: float
    ) -> ifcopenshell.entity_instance:
        """Create an IfcSweptDiskSolid.

        :param path_curve: A 3D curve entity defining the centreline path. Must have ``Dim == 3``.
        :param radius: Radius of the circular disk cross-section.
        :return: IfcSweptDiskSolid
        """
        if path_curve.Dim != 3:
            raise Exception(
                f"Path curve for IfcSweptDiskSolid should be 3D to be valid, currently it has {path_curve.Dim} dimensions.\n"
                "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcSweptDiskSolid.htm#8.8.3.42.4-Formal-propositions"
            )

        disk_solid = self.file.createIfcSweptDiskSolid(Directrix=path_curve, Radius=radius)
        return disk_solid

    def get_representation(
        self,
        context: ifcopenshell.entity_instance,
        items: Union[ifcopenshell.entity_instance, Sequence[ifcopenshell.entity_instance]],
        representation_type: Optional[str] = None,
    ) -> ifcopenshell.entity_instance:
        """Create IFC representation for the specified context and items.

        :param context: IfcGeometricRepresentationSubContext
        :param items: A single item or list of items, all of the same geometry category
        :param representation_type: Explicitly specified RepresentationType.
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
        return ifcopenshell.util.element.copy_deep(self.file, element)

    # UTILITIES
    def extrude_kwargs(self, axis: Literal["Y", "X", "Z"]) -> dict[str, tuple[float, float, float]]:
        """Shortcut to get kwargs for :meth:`extrude` to extrude along a principal axis.

        :param axis: The extrusion axis: ``'X'``, ``'Y'``, or ``'Z'``.
        :return: A dict with keys ``position_x_axis``, ``position_z_axis``, and ``extrusion_vector``.
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

        :param kwargs: A dict with ``position_x_axis`` and ``position_z_axis`` keys.
        :param angle: Rotation angle, in radians.
        :param counter_clockwise: If True, rotate counter-clockwise.
        :return: A new dict with rotated axes.
        """
        rot = np_rotation_matrix(-angle, 3, "Z")
        kwargs = kwargs.copy()
        kwargs["position_x_axis"] = rot @ kwargs["position_x_axis"]
        kwargs["position_z_axis"] = rot @ kwargs["position_z_axis"]
        return kwargs

    def get_polyline_coords(self, polyline: ifcopenshell.entity_instance) -> np.ndarray:
        """Extract the coordinate array from a polyline entity.

        :param polyline: An ``IfcIndexedPolyCurve`` or ``IfcPolyline`` entity.
        :return: Numpy array of the polyline's point coordinates.
        """
        coords = None
        if polyline.is_a("IfcIndexedPolyCurve"):
            coords = np.array(polyline.Points.CoordList)
        elif polyline.is_a("IfcPolyline"):
            coords = np.array(tuple(p.Coordinates for p in polyline.Points))
        else:
            raise Exception(f"Unsupported polyline type: {polyline.is_a()}")
        return coords

    def set_polyline_coords(self, polyline: ifcopenshell.entity_instance, coords: SequenceOfVectors) -> None:
        """Update the coordinates of a polyline entity in-place.

        :param polyline: An ``IfcIndexedPolyCurve`` or ``IfcPolyline`` entity.
        :param coords: New sequence of point coordinates.
        """
        if polyline.is_a("IfcIndexedPolyCurve"):
            polyline.Points.CoordList = ifc_safe_vector_type(coords)
        elif polyline.is_a("IfcPolyline"):
            ifc_points: list[ifcopenshell.entity_instance] = polyline.Points
            assert len(ifc_points) == len(coords)
            for point, co in zip(ifc_points, ifc_safe_vector_type(coords)):
                point.Coordinates = co
        else:
            raise Exception(f"Unsupported polyline type: {polyline.is_a()}")

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

        :param coords:           list of 2d coords.
        :param fillets:          list of points from `coords` to base fillet on.
        :param fillet_radius:    list of fillet radius for each of corresponding point from `fillets`.
        :param closed:           boolean whether curve should be closed.
        :param create_ifc_curve: create IfcIndexedPolyCurve or just return the data.
        :return: (points, segments, ifc_curve)
        """

        def remove_redundant_points(
            points: list[VectorType], segments: list[list[int]]
        ) -> tuple[list[VectorType], list[list[int]]]:
            points = [tuple(p) for p in points]
            segments = segments.copy()

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

            valid_segments = [segment for segment in segments if len(set(segment)) != 1]
            points = [point for i, point in enumerate(points) if i not in points_to_remove]
            unique_points = sorted(set(chain(*valid_segments)))
            unique_points_translation = {prev: i for i, prev in enumerate(unique_points)}
            valid_segments = [[unique_points_translation[p] for p in s] for s in valid_segments]

            return points, valid_segments

        if isinstance(fillet_radius, (float, int)):
            fillet_radius = [fillet_radius] * len(fillets)

        fillets: dict[int, float] = dict(zip(fillets, fillet_radius))
        segments: list[list[int]] = []
        points: list[VectorType] = []

        for co_i, co in enumerate(coords, 0):
            current_point = len(points)
            if co_i in fillets:
                r = fillets[co_i]
                rsb = r * cos(pi / 4)
                rss = r - rsb

                next_co = coords[(co_i + 1) % len(coords)]
                previous_co = coords[co_i - 1]

                x_direction = 1 if coords[co_i][0] < previous_co[0] or coords[co_i][0] < next_co[0] else -1
                y_direction = 1 if coords[co_i][1] < previous_co[1] or coords[co_i][1] < next_co[1] else -1

                xshift_point = (co[0] + r * x_direction, co[1])
                middle_point = (co[0] + rss * x_direction, co[1] + rss * y_direction)
                yshift_point = (co[0], co[1] + r * y_direction)

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

        if segments[0][0] == -1:
            segments[0][0] = len(points) - 1

        points, segments = remove_redundant_points(points, segments)
        ifc_curve = None
        if create_ifc_curve:
            ifc_points = self.file.createIfcCartesianPointList2D(ifc_safe_vector_type(points))
            ifc_segments = []
            for segment in segments:
                segment = [i + 1 for i in segment]
                if len(segment) == 2:
                    ifc_segments.append(self.file.createIfcLineIndex(segment))
                elif len(segment) == 3:
                    ifc_segments.append(self.file.createIfcArcIndex(segment))

            ifc_curve = self.file.createIfcIndexedPolyCurve(Points=ifc_points, Segments=ifc_segments)
        return (points, segments, ifc_curve)

    def create_transition_arc_ifc(
        self, width: float, height: float, create_ifc_curve: bool = False
    ) -> tuple[SequenceOfVectors, list[list[int]], Union[ifcopenshell.entity_instance, None]]:
        """Create an arc fitting inside a rectangle of the given width and height.

        :param width: Width of the bounding rectangle.
        :param height: Height of the bounding rectangle.
        :param create_ifc_curve: If True, also create an IfcIndexedPolyCurve.
        :return: A tuple ``(points, segments, ifc_curve)``.
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

        :param points: List of 3D coordinates.
        :param faces: List of faces, each face a sequence of zero-based point indices.
        :return: IfcFacetedBrep (IFC2X3) or IfcPolygonalFaceSet (IFC4+).
        """
        if self.file.schema == "IFC2X3":
            return self.faceted_brep(points, faces)
        return self.polygonal_face_set(points, faces)

    def faceted_brep(self, points: SequenceOfVectors, faces: Sequence[Sequence[int]]) -> ifcopenshell.entity_instance:
        """Generate an IfcFacetedBrep with a closed shell

        :param points: list of 3d coordinates
        :param faces: list of faces consisted of point indices (starting from 0)
        :return: IfcFacetedBrep
        """
        verts = [self.file.createIfcCartesianPoint(p) for p in ifc_safe_vector_type(points)]
        faces: list[ifcopenshell.entity_instance] = [
            self.file.createIfcFace(
                [self.file.createIfcFaceOuterBound(self.file.createIfcPolyLoop([verts[v] for v in f]), True)]
            )
            for f in faces
        ]
        return self.file.createIfcFacetedBrep(self.file.createIfcClosedShell(faces))

    def triangulated_face_set(
        self, points: SequenceOfVectors, faces: Sequence[Sequence[int]]
    ) -> ifcopenshell.entity_instance:
        """
        Generate an IfcTriangulatedFaceSet

        :param points: list of 3d coordinates
        :param faces: list of triangles consisted of point indices (starting from 0)
        :return: IfcTriangulatedFaceSet
        """
        ifc_points = self.file.createIfcCartesianPointList3D(ifc_safe_vector_type(points))
        ifc_faces = [[i + 1 for i in face][:3] for face in faces]
        return self.file.createIfcTriangulatedFaceSet(Coordinates=ifc_points, CoordIndex=ifc_faces)

    def polygonal_face_set(
        self, points: SequenceOfVectors, faces: Sequence[Union[Sequence[int], Sequence[Sequence[int]]]]
    ) -> ifcopenshell.entity_instance:
        """
        Generate an IfcPolygonalFaceSet

        :param points: list of 3d coordinates
        :param faces: list of faces consisted of point indices (starting from 0)
                      in case of multiple sequences per face, the subsequent ones are inner voids
        :return: IfcPolygonalFaceSet
        """

        def is_sequence_of_ints(x):
            return isinstance(x, Sequence) and not isinstance(x, (str, bytes)) and all(isinstance(el, int) for el in x)

        def is_sequence_of_sequence_of_ints(x):
            return (
                isinstance(x, Sequence) and not isinstance(x, (str, bytes)) and all(is_sequence_of_ints(el) for el in x)
            )

        def incr(face):
            return [i + 1 for i in face]

        if not all(is_sequence_of_ints(f) or is_sequence_of_sequence_of_ints(f) for f in faces):
            raise ValueError("Expected a sequence of int or sequence of sequence of int for each face")

        ifc_points = self.file.createIfcCartesianPointList3D(ifc_safe_vector_type(points))
        ifc_faces = [
            (
                self.file.createIfcIndexedPolygonalFace(incr(face))
                if is_sequence_of_ints(face)
                else self.file.createIfcIndexedPolygonalFaceWithVoids(incr(face[0]), list(map(incr, face[1:])))
            )
            for face in faces
        ]
        return self.file.createIfcPolygonalFaceSet(Coordinates=ifc_points, Faces=ifc_faces)

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

        :param points: list of points, assuming they form consecutive closed polyline.
        :param magnitude: extrusion magnitude
        :param extrusion_vector: extrusion direction.
        :param offset: offset from the points
        :param start_cap: if True, create start cap.
        :param end_cap: if True, create end cap.
        :return: IfcPolygonalFaceSet
        """

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
        faces.append((last_vert_i, 0, n_verts + 0, n_verts + last_vert_i))

        if end_cap:
            faces.append(tuple(range(n_verts, n_verts * 2)))
        if start_cap:
            faces.append(tuple(reversed(range(n_verts))))

        face_set = self.polygonal_face_set(all_points, faces)
        return face_set

    # --- MEP methods (Bonsai-specific, not ported) ---

    def create_z_profile_lips_curve(self, *args: Any, **kwargs: Any) -> Any:
        raise NotImplementedError("Not ported to ifcapi")

    def mep_transition_shape(self, *args: Any, **kwargs: Any) -> Any:
        raise NotImplementedError("Not ported to ifcapi")

    def mep_transition_length(self, *args: Any, **kwargs: Any) -> Any:
        raise NotImplementedError("Not ported to ifcapi")

    def mep_transition_calculate(self, *args: Any, **kwargs: Any) -> Any:
        raise NotImplementedError("Not ported to ifcapi")

    def mep_bend_shape(self, *args: Any, **kwargs: Any) -> Any:
        raise NotImplementedError("Not ported to ifcapi")
