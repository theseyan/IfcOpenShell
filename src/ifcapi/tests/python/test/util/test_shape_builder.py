# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2023 Dion Moult <dion@thinkmoult.com>, @Andrej730
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

from math import degrees, radians
from typing import Any, Union

import numpy as np
import pytest

import ifcopenshell.geom
import ifcopenshell.util.shape
import test.bootstrap
from ifcopenshell.util.shape_builder import (
    ShapeBuilder,
    V,
    is_x,
    np_angle,
    np_angle_signed,
    np_intersect_line_line,
    np_matrix_to_euler,
    np_normal,
    np_rotation_matrix,
    np_to_3d,
)


class TestMathutilsCompatibleMethods(test.bootstrap.IFC4):
    def test_np_rotation_matrix(self):
        pytest.importorskip("mathutils")
        from mathutils import Matrix, Vector  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]

        # 2D.
        assert np.allclose(Matrix.Rotation(radians(45), 2), np_rotation_matrix(radians(45), 2))
        assert np.allclose(Matrix.Rotation(radians(45), 2, "Z"), np_rotation_matrix(radians(45), 2, "Z"))

        # 3D.
        assert np.allclose(Matrix.Rotation(radians(45), 3, "X"), np_rotation_matrix(radians(45), 3, "X"))
        assert np.allclose(Matrix.Rotation(radians(45), 3, "Y"), np_rotation_matrix(radians(45), 3, "Y"))
        assert np.allclose(Matrix.Rotation(radians(45), 3, "Z"), np_rotation_matrix(radians(45), 3, "Z"))
        rotation_vector_args = radians(45), 3, Vector((1, 1, 1)).normalized()
        assert np.allclose(Matrix.Rotation(*rotation_vector_args), np_rotation_matrix(*rotation_vector_args))

        # Size 4.
        assert np.allclose(Matrix.Rotation(radians(45), 4, "X"), np_rotation_matrix(radians(45), 4, "X"))
        assert np.allclose(Matrix.Rotation(radians(45), 4, "Y"), np_rotation_matrix(radians(45), 4, "Y"))
        assert np.allclose(Matrix.Rotation(radians(45), 4, "Z"), np_rotation_matrix(radians(45), 4, "Z"))
        rotation_vector_args = radians(45), 4, Vector((1, 1, 1)).normalized()
        assert np.allclose(Matrix.Rotation(*rotation_vector_args), np_rotation_matrix(*rotation_vector_args))

    def test_np_matrix_to_euler(self):
        pytest.importorskip("mathutils")
        from mathutils import Euler  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]

        # Test 3x3.
        rot = Euler((0.5, 0.5, 0.5)).to_matrix()
        assert np.allclose(rot.to_euler(), np_matrix_to_euler(V(rot)))

        rot = rot.to_4x4()
        assert np.allclose(rot.to_euler(), np_matrix_to_euler(V(rot)))

        # Ensure support scaled matrices.
        rot = Euler((0.5, 0.5, 0.5)).to_matrix()
        rot.col[0] *= 2
        assert np.allclose(rot.to_euler(), np_matrix_to_euler(V(rot)))

    def test_np_angle(self):
        pytest.importorskip("mathutils")
        from mathutils import Vector  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]

        v1, v2 = (1, 0, 0), (0, 1, 0)
        angle = np_angle(v1, v2)
        assert is_x(angle, Vector(v1).angle(Vector(v2)))
        assert is_x(angle, radians(90))

        v1, v2 = v1[:2], v2[:2]
        angle = np_angle_signed(v1, v2)
        assert is_x(angle, Vector(v1).angle_signed(Vector(v2)))
        assert is_x(angle, -radians(90))

        v1, v2 = (0, 1, 0), (1, 0, 0)
        angle = np_angle(v1, v2)
        assert is_x(angle, Vector(v1).angle(Vector(v2)))
        assert is_x(angle, radians(90))

        v1, v2 = v1[:2], v2[:2]
        angle = np_angle_signed(v1, v2)
        assert is_x(angle, Vector(v1).angle_signed(Vector(v2)))
        assert is_x(angle, radians(90))

    def test_np_normal(self):
        pytest.importorskip("mathutils")
        import mathutils.geometry  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]

        vectors = (0, 0, 0), (1, 0, 0), (0, 1, 0)
        n = mathutils.geometry.normal(vectors)
        assert np.allclose(n, np_normal(vectors))
        assert np.allclose(n, (0, 0, 1))

        vectors = (0, 0, 0), (0, 1, 0), (1, 0, 0)
        n = mathutils.geometry.normal(vectors)
        assert np.allclose(n, np_normal(vectors))
        assert np.allclose(n, (0, 0, -1))

    def test_np_intersect_line_line(self):
        pytest.importorskip("mathutils")
        import mathutils.geometry  # pyright: ignore[reportMissingImports]  # ty:ignore[unresolved-import]

        p1, p2 = [0, 0, 0], [1, 1, 1]
        q1, q2 = [0, 1, 0], [1, 0, 1]
        expected = mathutils.geometry.intersect_line_line(tuple(p1), tuple(p2), tuple(q1), tuple(q2))
        result = np_intersect_line_line(p1, p2, q1, q2)
        assert np.allclose(expected, result)


class TestRectangle(test.bootstrap.IFC4):
    def test_get_rectangle_coords(self):
        builder = ShapeBuilder(self.file)

        # 2D.
        coords = builder.get_rectangle_coords((1, 2), (3, 4))
        assert np.allclose(coords, [[3.0, 4.0], [4.0, 4.0], [4.0, 6.0], [3.0, 6.0]])

        # 3D, XY plane.
        coords = builder.get_rectangle_coords((1, 2, 0), (3, 4, 0))
        assert np.allclose(coords, [[3.0, 4.0, 0.0], [4.0, 4.0, 0.0], [4.0, 6.0, 0.0], [3.0, 6.0, 0.0]])

        # 3D, XZ plane.
        coords = builder.get_rectangle_coords((1, 0, 2), (3, 0, 4))
        assert np.allclose(coords, [[3.0, 0.0, 4.0], [4.0, 0.0, 4.0], [4.0, 0.0, 6.0], [3.0, 0.0, 6.0]])


class TestCreatePolyline(test.bootstrap.IFC4):
    def test_simple_polyline(self):
        builder = ShapeBuilder(self.file)

        # rectangle
        points = V([(0.0, 0.0), (1.0, 0.0), (1.0, 1.0), (0.0, 1.0)])
        position = (2.0, 0.0)
        polyline = builder.polyline(points, closed=True, position_offset=position)

        points += position
        assert np.allclose(points, polyline.Points.CoordList)
        # use 1 line index if there are no arcs
        assert len(polyline.Segments) == 1
        segment = polyline.Segments[0]
        assert segment.is_a("IfcLineIndex")
        assert segment.wrappedValue == (1, 2, 3, 4, 1)

    def test_polyline_with_arc(self):
        builder = ShapeBuilder(self.file)

        points = V([(1, 0), (0.707, 0.707), (0, 1), (0, 2)])
        position = (2, 0)
        arc_points = (1,)
        # 4=IfcIndexedPolyCurve(# 3,(IfcArcIndex((1,2,3)),IfcLineIndex((3,4,1))),$)
        polyline = builder.polyline(points, closed=False, position_offset=position, arc_points=arc_points)
        points += position
        assert np.allclose(points, polyline.Points.CoordList)
        assert len(polyline.Segments) == 2

        segment = polyline.Segments[0]
        assert segment.is_a("IfcArcIndex")
        assert segment.wrappedValue == (1, 2, 3)

        segment = polyline.Segments[1]
        assert segment.is_a("IfcLineIndex")
        assert segment.wrappedValue == (3, 4)

    def test_closed_polyline_ending_with_arc(self):
        builder = ShapeBuilder(self.file)

        points = V([(0, 0), (1, 0), (0.5, 0.5)])
        position = (2, 0)
        arc_points = (2,)
        # 4=IfcIndexedPolyCurve(#3,(IfcLineIndex((1,2)),IfcArcIndex((2,3,1))),$)
        polyline = builder.polyline(points, closed=True, position_offset=position, arc_points=arc_points)
        points += position
        assert np.allclose(points, polyline.Points.CoordList)
        assert len(polyline.Segments) == 2

        segment = polyline.Segments[0]
        assert segment.is_a("IfcLineIndex")
        assert segment.wrappedValue == (1, 2)

        segment = polyline.Segments[1]
        assert segment.is_a("IfcArcIndex")
        assert segment.wrappedValue == (2, 3, 1)


class TestPrimitiveCurves(test.bootstrap.IFC4):
    def test_circle(self):
        builder = ShapeBuilder(self.file)

        circle = builder.circle(center=(2.0, 3.0), radius=4.0)

        assert circle.is_a("IfcCircle")
        assert np.allclose(circle.Position.Location.Coordinates, (2.0, 3.0))
        assert circle.Radius == pytest.approx(4.0)

    def test_plane(self):
        builder = ShapeBuilder(self.file)

        plane = builder.plane(location=(1.0, 2.0, 3.0), normal=(0.0, 0.0, 1.0))

        assert plane.is_a("IfcPlane")
        assert np.allclose(plane.Position.Location.Coordinates, (1.0, 2.0, 3.0))
        assert np.allclose(plane.Position.Axis.DirectionRatios, (0.0, 0.0, 1.0))

    def test_curve_between_two_points(self):
        builder = ShapeBuilder(self.file)

        curve = builder.curve_between_two_points(((0.0, 0.0), (1.0, 1.0)))

        assert curve.is_a("IfcIndexedPolyCurve")
        assert curve.Segments[0].is_a("IfcArcIndex")
        assert curve.Segments[0].wrappedValue == (1, 2, 3)

    def test_curve_between_two_points_rejects_invalid_input(self):
        builder = ShapeBuilder(self.file)

        with pytest.raises(ValueError, match="expects two 2D points"):
            builder.curve_between_two_points(((0.0, 0.0),))
        with pytest.raises(ValueError, match="expects two 2D points"):
            builder.curve_between_two_points(((0.0, 0.0, 0.0), (1.0, 1.0, 1.0)))

    def test_axis2_placements(self):
        builder = ShapeBuilder(self.file)

        placement_2d = builder.create_axis2_placement_2d((1.0, 2.0), (0.0, 1.0))
        placement_3d = builder.create_axis2_placement_3d((1.0, 2.0, 3.0), (0.0, 1.0, 0.0), (1.0, 0.0, 0.0))

        assert placement_2d.is_a("IfcAxis2Placement2D")
        assert np.allclose(placement_2d.Location.Coordinates, (1.0, 2.0))
        assert np.allclose(placement_2d.RefDirection.DirectionRatios, (0.0, 1.0))
        assert placement_3d.is_a("IfcAxis2Placement3D")
        assert np.allclose(placement_3d.Location.Coordinates, (1.0, 2.0, 3.0))
        assert np.allclose(placement_3d.Axis.DirectionRatios, (0.0, 1.0, 0.0))
        assert np.allclose(placement_3d.RefDirection.DirectionRatios, (1.0, 0.0, 0.0))

    def test_ellipse_curve(self):
        builder = ShapeBuilder(self.file)

        ellipse = builder.create_ellipse_curve(2.0, 1.0, position=(3.0, 4.0), ref_x_direction=(0.0, 1.0))

        assert ellipse.is_a("IfcEllipse")
        assert ellipse.SemiAxis1 == pytest.approx(2.0)
        assert ellipse.SemiAxis2 == pytest.approx(1.0)
        assert np.allclose(ellipse.Position.Location.Coordinates, (3.0, 4.0))
        assert np.allclose(ellipse.Position.RefDirection.DirectionRatios, (0.0, 1.0))


class TestMirror(test.bootstrap.IFC4):
    def test_mirror(self):
        builder = ShapeBuilder(self.file)
        rectangle = builder.rectangle(size=(100, 100))
        assert np.allclose(rectangle.Points.CoordList, ((0.0, 0.0), (100.0, 0.0), (100.0, 100.0), (0.0, 100.0)))
        builder.mirror(rectangle, mirror_axes=(1, 0))
        assert np.allclose(rectangle.Points.CoordList, ((0.0, 0.0), (-100.0, 0.0), (-100.0, 100.0), (0.0, 100.0)))

    def test_mirror_polyline_uses_placement_matrix(self):
        builder = ShapeBuilder(self.file)
        polyline = builder.polyline([(1.0, 0.0), (2.0, 0.0)])
        placement_matrix = np_rotation_matrix(radians(90), 3, "Z")

        builder.mirror(polyline, mirror_axes=(1, 0), placement_matrix=placement_matrix)

        assert np.allclose(polyline.Points.CoordList, ((1.0, 0.0), (2.0, 0.0)))

    def test_mirror_create_copy_keeps_original(self):
        builder = ShapeBuilder(self.file)
        rectangle = builder.rectangle(size=(100, 100))

        mirrored = builder.mirror(rectangle, mirror_axes=(1, 0), create_copy=True)

        assert mirrored != rectangle
        assert np.allclose(rectangle.Points.CoordList, ((0.0, 0.0), (100.0, 0.0), (100.0, 100.0), (0.0, 100.0)))
        assert np.allclose(mirrored.Points.CoordList, ((0.0, 0.0), (-100.0, 0.0), (-100.0, 100.0), (0.0, 100.0)))


class TestTranslate(test.bootstrap.IFC4):
    def test_translate_create_copy_keeps_original(self):
        builder = ShapeBuilder(self.file)
        rectangle = builder.rectangle(size=(100, 100))

        translated = builder.translate(rectangle, (2.0, 3.0), create_copy=True)

        assert translated != rectangle
        assert np.allclose(rectangle.Points.CoordList, ((0.0, 0.0), (100.0, 0.0), (100.0, 100.0), (0.0, 100.0)))
        assert np.allclose(translated.Points.CoordList, ((2.0, 3.0), (102.0, 3.0), (102.0, 103.0), (2.0, 103.0)))


class TestRotate(test.bootstrap.IFC4):
    def test_rotate_create_copy_keeps_original(self):
        builder = ShapeBuilder(self.file)
        rectangle = builder.rectangle(size=(1, 2))

        rotated = builder.rotate(rectangle, 90, create_copy=True)

        assert rotated != rectangle
        assert np.allclose(rectangle.Points.CoordList, ((0.0, 0.0), (1.0, 0.0), (1.0, 2.0), (0.0, 2.0)))
        assert np.allclose(rotated.Points.CoordList, ((0.0, 0.0), (0.0, -1.0), (2.0, -1.0), (2.0, 0.0)))


class TestDeepCopy(test.bootstrap.IFC4):
    def test_deep_copy_preserves_nested_entity_lists(self):
        builder = ShapeBuilder(self.file)
        surface = self.file.create_entity("IfcBSplineSurfaceWithKnots")
        points = [self.file.create_entity("IfcCartesianPoint", (float(i), 0.0, 0.0)) for i in range(4)]
        surface.ControlPointsList = (points[:2], points[2:])

        copied = builder.deep_copy(surface)

        assert copied != surface
        assert copied.ControlPointsList != surface.ControlPointsList
        assert [[point.Coordinates for point in row] for row in copied.ControlPointsList] == [
            [(0.0, 0.0, 0.0), (1.0, 0.0, 0.0)],
            [(2.0, 0.0, 0.0), (3.0, 0.0, 0.0)],
        ]
        assert copied.ControlPointsList[0][0] != surface.ControlPointsList[0][0]


class TestVertex(test.bootstrap.IFC4):
    def test_run(self):
        builder = ShapeBuilder(self.file)
        vertex = builder.vertex((1, 2, 3))
        assert np.allclose(vertex.VertexGeometry.Coordinates, (1, 2, 3))


class TestEdge(test.bootstrap.IFC4):
    def test_run(self):
        builder = ShapeBuilder(self.file)
        edge = builder.edge((1, 0, 0), (1, 2, 3))
        assert np.allclose(edge.EdgeStart.VertexGeometry.Coordinates, (1, 0, 0))
        assert np.allclose(edge.EdgeEnd.VertexGeometry.Coordinates, (1, 2, 3))


class TestFace(test.bootstrap.IFC4):
    def test_run(self):
        builder = ShapeBuilder(self.file)
        face = builder.face(((0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)))
        assert np.allclose(face.Bounds[0].Bound.Polygon[0], (0, 0, 0))
        assert np.allclose(face.Bounds[0].Bound.Polygon[1], (1, 0, 0))
        assert np.allclose(face.Bounds[0].Bound.Polygon[2], (1, 1, 0))
        assert np.allclose(face.Bounds[0].Bound.Polygon[3], (0, 1, 0))


class TestProfile(test.bootstrap.IFC4):
    def test_single_inner_curve_is_accepted(self):
        builder = ShapeBuilder(self.file)
        outer_curve = builder.rectangle(size=(4.0, 4.0))
        inner_curve = builder.rectangle(size=(1.0, 1.0), position=(1.0, 1.0))

        profile = builder.profile(outer_curve, inner_curves=inner_curve)

        assert profile.is_a("IfcArbitraryProfileDefWithVoids")
        assert profile.OuterCurve == outer_curve
        assert profile.InnerCurves == (inner_curve,)

    def test_3d_outer_curve_is_rejected(self):
        builder = ShapeBuilder(self.file)
        outer_curve = builder.polyline([(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (1.0, 1.0, 0.0)], closed=True)

        with pytest.raises(Exception, match="Outer curve .* should be 2D"):
            builder.profile(outer_curve)


class TestCalculateTransitions(test.bootstrap.IFC4):
    def calculate_and_test(self, params: dict[str, Any], length: Union[float, None]):
        np_X, np_Y = 0, 1
        np_XY = slice(2)
        np_YX = [1, 0]

        end_profile = params["end_profile"]
        start_half_dim: np.ndarray = params["start_half_dim"]
        end_half_dim: np.ndarray = params["end_half_dim"]
        offset: np.ndarray = params["offset"]
        offset = offset if not end_profile else offset[np_YX]
        angle = params["angle"]

        calculated_length = self.builder.mep_transition_calculate(**params)
        if length is None:
            assert calculated_length is None
            return

        assert calculated_length is not None and is_x(calculated_length, length)

        # angle confirmation methods:
        # A - between two profiles of different dimensions
        # B - between two profiles of same dimensions, no offset by x
        # C - between two profiles of same dimensions, has offset by x
        diff = np.subtract(start_half_dim[np_XY], end_half_dim[np_XY])
        same_dimension = is_x(diff[np_X] if not end_profile else diff[np_Y], 0)
        if not same_dimension:
            confirmation_method = "A"
        else:
            confirmation_method = "B" if is_x(offset[np_X], 0) else "C"

        if confirmation_method == "A":
            A = (end_half_dim if end_profile else start_half_dim) * (1, 0, 0)
            end_profile_offset = np_to_3d(offset, length)
            D = (start_half_dim if end_profile else end_half_dim) * (1, 0, 0)
            B, C = -A, -D
            C += end_profile_offset
            D += end_profile_offset
            tested_angle = degrees(np_angle(A - D, B - C))
            assert is_x(tested_angle, angle)

        elif confirmation_method == "B":
            O = np.zeros(3)
            A = (-start_half_dim[np_X], 0, length) + np_to_3d(offset)
            B = A * (-1, 1, 1)
            tested_angle = degrees(np_angle(A - O, B - O))
            assert is_x(tested_angle, angle)

        elif confirmation_method == "C":
            A = V(-start_half_dim[np_X], 0, 0)
            H = A + (0, 0, length)
            H[np_Y] += offset[np_Y]
            D = H.copy()
            D[np_X] += offset[np_X]
            tested_angle = degrees(np_angle(H - A, D - A))
            assert is_x(tested_angle, angle)

        calculated_angle = self.builder.mep_transition_calculate(
            **params | {"angle": None, "length": calculated_length}
        )
        assert calculated_angle is not None
        assert is_x(calculated_angle, angle)

    def test_mep_transition_same_dims_no_offset(self):
        self.builder = ShapeBuilder(self.file)
        params = {
            "start_half_dim": V(100, 50, 0),
            "end_half_dim": V(100, 50, 0),
            "offset": V(0, 0),
            "end_profile": False,
            "angle": 90,
            "verbose": True,
        }
        self.calculate_and_test(params, 100)

    def test_mep_transition_same_dims_has_x_offset(self):
        self.builder = ShapeBuilder(self.file)
        params = {
            "start_half_dim": V(100, 50, 0),
            "end_half_dim": V(100, 50, 0),
            "offset": V(50, 50),
            "end_profile": False,
            "angle": 30,
            "verbose": True,
        }
        self.calculate_and_test(params, 70.71068)

    def test_mep_transition_same_dims_has_y_offset(self):
        self.builder = ShapeBuilder(self.file)
        params = {
            "start_half_dim": V(100, 50, 0),
            "end_half_dim": V(100, 50, 0),
            "offset": V(0, 50),
            "end_profile": False,
            "angle": 90,
            "verbose": True,
        }
        self.calculate_and_test(params, 86.60254)

    def test_mep_transition_diff_dims_no_offset(self):
        self.builder = ShapeBuilder(self.file)
        params = {
            "start_half_dim": V(100, 50, 0),
            "end_half_dim": V(50, 100, 0),
            "offset": V(0, 0),
            "end_profile": False,
            "angle": 30,
            "verbose": True,
        }
        self.calculate_and_test(params, 186.60254)

    def test_mep_transition_diff_dims_has_x_y_offset(self):
        self.builder = ShapeBuilder(self.file)
        params = {
            "start_half_dim": V(100, 50, 0),
            "end_half_dim": V(50, 100, 0),
            "offset": V(50, 50),
            "end_profile": False,
            "angle": 30,
            "verbose": True,
        }
        self.calculate_and_test(params, 165.83124)

    def test_mep_transition_length_preserves_sub_tenth_offset_precision(self):
        self.builder = ShapeBuilder(self.file)

        calculated_length = self.builder.mep_transition_length(
            V(100, 50, 0), V(50, 100, 0), angle=30, profile_offset=V(50.04, 50.04), verbose=False
        )

        assert calculated_length is not None
        assert is_x(calculated_length, 165.79504)

    def test_mep_transition_y_offset_too_big(self):
        self.builder = ShapeBuilder(self.file)

        # method A
        params = {
            "start_half_dim": V(100, 50, 0),
            "end_half_dim": V(50, 100, 0),
            # offset.y > h - 190 > 186.6
            "offset": V(0, 190),
            "end_profile": False,
            "angle": 30,
            "verbose": True,
        }
        self.calculate_and_test(params, None)

        # method B
        params["end_half_dim"] = V(100, 100, 0)
        self.calculate_and_test(params, None)

        # method C
        params["offset"][0] = 10.0
        self.calculate_and_test(params, None)


class TestMepBendShape(test.bootstrap.IFC4):
    def test_missing_material_profile_raises(self):
        builder = ShapeBuilder(self.file)
        segment = self.file.create_entity("IfcFlowSegment")

        with pytest.raises(ValueError, match="segment must have a single material profile"):
            builder.mep_bend_shape(segment, 1.0, 1.0, 1.0, 1.0, (1.0, 0.0), False)

    def test_zero_bend_vector_raises(self):
        builder = ShapeBuilder(self.file)
        segment = self.file.create_entity("IfcFlowSegment")
        profile = self.file.create_entity("IfcRectangleProfileDef", ProfileType="AREA", XDim=2.0, YDim=1.0)
        material = self.file.create_entity("IfcMaterial", Name="material")
        material_profile = self.file.create_entity("IfcMaterialProfile", Material=material, Profile=profile)
        material_set = self.file.create_entity("IfcMaterialProfileSet", MaterialProfiles=[material_profile])
        self.file.create_entity(
            "IfcRelAssociatesMaterial",
            GlobalId="0",
            RelatedObjects=[segment],
            RelatingMaterial=material_set,
        )

        with pytest.raises(ValueError, match="bend_vector must have a non-zero X or Y component"):
            builder.mep_bend_shape(segment, 1.0, 1.0, 1.0, 1.0, (0.0, 0.0), False)

        representation, bend_data = builder.mep_bend_shape(segment, 1.0, 1.0, 1.0, 1.0, (1.0, 0.0), False)
        assert representation.is_a("IfcShapeRepresentation")
        assert bend_data["lateral_axis"] == 0


class TestFaceset(test.bootstrap.IFC4):
    @pytest.mark.parametrize("with_inner", [False, True])
    def test_polygonal_face_set_simple_and_with_voids(self, with_inner):
        self.builder = ShapeBuilder(self.file)

        v0 = (0.0, 0.0, 0.0)
        v1 = (4.0, 0.0, 0.0)
        v2 = (4.0, 4.0, 0.0)
        v3 = (0.0, 4.0, 0.0)

        v4 = (1.0, 1.0, 0.0)
        v5 = (3.0, 1.0, 0.0)
        v6 = (3.0, 3.0, 0.0)
        v7 = (1.0, 3.0, 0.0)

        if with_inner:
            points = [v0, v1, v2, v3, v4, v5, v6, v7]

            faces = [
                [[0, 1, 2, 3], [4, 5, 6, 7]],  # outer loop with inner hole
            ]
        else:
            points = [v0, v1, v2, v3]

            faces = [[0, 1, 2, 3]]  # only outer loop

        result = self.builder.polygonal_face_set(points, faces)

        assert result.is_a("IfcPolygonalFaceSet")
        assert result.Coordinates.is_a("IfcCartesianPointList3D")
        assert len(result.Faces) == 1
        if with_inner:
            assert result.Faces[0].is_a("IfcIndexedPolygonalFaceWithVoids")
        else:
            assert result.Faces[0].is_a("IfcIndexedPolygonalFace")

        shp = ifcopenshell.geom.create_shape(ifcopenshell.geom.settings(), result)
        if with_inner:
            assert ifcopenshell.util.shape.get_area(shp) == pytest.approx(12.0)
        else:
            assert ifcopenshell.util.shape.get_area(shp) == pytest.approx(16.0)

    def test_polygonal_face_set_invalid_face_types(self):
        self.builder = ShapeBuilder(self.file)
        with pytest.raises(ValueError, match="Expected a sequence of int or sequence of sequence of int"):
            self.builder.polygonal_face_set([], ["123"])
        with pytest.raises(ValueError, match="Expected a sequence of int or sequence of sequence of int"):
            self.builder.polygonal_face_set([], [[1.0, 2.0, 3.0]])
        with pytest.raises(ValueError, match="Expected a sequence of int or sequence of sequence of int"):
            self.builder.polygonal_face_set([], [[[[1, 2], 3], [4, 5, 6]]])

    def test_polygonal_face_set_rejects_empty_face(self):
        self.builder = ShapeBuilder(self.file)
        with pytest.raises(ValueError, match="polygonal face loop must contain at least one index"):
            self.builder.polygonal_face_set([], [[]])
