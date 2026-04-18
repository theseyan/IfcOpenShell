# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
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

import numpy as np
import pytest

import ifcopenshell.util.placement as subject
import test.bootstrap


class TestGetStoreyElevationIFC4(test.bootstrap.IFC4):
    def test_run(self):
        storey = self.file.createIfcBuildingStorey()
        placement = self.file.createIfcLocalPlacement()
        placement.RelativePlacement = self.file.createIfcAxis2Placement3D(
            self.file.createIfcCartesianPoint((0.0, 0.0, 3.0))
        )
        storey.ObjectPlacement = placement
        assert subject.get_storey_elevation(storey) == 3.0

    def test_getting_the_elevation_if_no_z_location(self):
        storey = self.file.createIfcBuildingStorey()
        storey.Elevation = 3.0
        assert subject.get_storey_elevation(storey) == 3.0

    def test_returning_0_as_a_fallback(self):
        storey = self.file.createIfcBuildingStorey()
        assert subject.get_storey_elevation(storey) == 0.0
        building = self.file.createIfcBuilding()
        assert subject.get_storey_elevation(building) == 0.0


class TestA2P:
    def test_identity(self):
        m = subject.a2p((0, 0, 0), (0, 0, 1), (1, 0, 0))
        np.testing.assert_array_almost_equal(m, np.eye(4))

    def test_translation(self):
        m = subject.a2p((1, 2, 3), (0, 0, 1), (1, 0, 0))
        assert m[0, 3] == 1
        assert m[1, 3] == 2
        assert m[2, 3] == 3

    def test_rotation_90_around_z(self):
        m = subject.a2p((0, 0, 0), (0, 0, 1), (0, 1, 0))
        np.testing.assert_array_almost_equal(m[0, 0], 0)
        np.testing.assert_array_almost_equal(m[1, 0], 1)
        np.testing.assert_array_almost_equal(m[0, 1], -1)
        np.testing.assert_array_almost_equal(m[1, 1], 0)


class TestGetLocalPlacementIFC4(test.bootstrap.IFC4):
    def test_none_returns_identity(self):
        np.testing.assert_array_almost_equal(subject.get_local_placement(None), np.eye(4))

    def test_simple_translation(self):
        pt = self.file.createIfcCartesianPoint((5.0, 6.0, 7.0))
        p = self.file.createIfcLocalPlacement(
            RelativePlacement=self.file.createIfcAxis2Placement3D(pt)
        )
        m = subject.get_local_placement(p)
        assert m[0, 3] == 5.0
        assert m[1, 3] == 6.0
        assert m[2, 3] == 7.0

    def test_nested_placement_composes(self):
        pt_a = self.file.createIfcCartesianPoint((1.0, 0.0, 0.0))
        pt_b = self.file.createIfcCartesianPoint((0.0, 2.0, 0.0))
        a = self.file.createIfcLocalPlacement(
            RelativePlacement=self.file.createIfcAxis2Placement3D(pt_a)
        )
        b = self.file.createIfcLocalPlacement(
            PlacementRelTo=a,
            RelativePlacement=self.file.createIfcAxis2Placement3D(pt_b),
        )
        m = subject.get_local_placement(b)
        assert m[0, 3] == 1.0
        assert m[1, 3] == 2.0
        assert m[2, 3] == 0.0


class TestAxis2Placement2DIFC4(test.bootstrap.IFC4):
    def test_basic(self):
        plc = self.file.createIfcAxis2Placement2D(
            self.file.createIfcCartesianPoint((3.0, 4.0))
        )
        m = subject.get_axis2placement(plc)
        assert m[0, 3] == 3.0
        assert m[1, 3] == 4.0
        assert m[2, 3] == 0.0


class TestRotation:
    def test_90_around_z_degrees(self):
        m = subject.rotation(90.0, "Z", is_degrees=True)
        np.testing.assert_array_almost_equal(m[0, 0], 0.0)
        np.testing.assert_array_almost_equal(m[1, 0], 1.0)
        np.testing.assert_array_almost_equal(m[0, 1], -1.0)
        np.testing.assert_array_almost_equal(m[1, 1], 0.0)

    def test_radians(self):
        m = subject.rotation(np.pi / 2, "X", is_degrees=False)
        np.testing.assert_array_almost_equal(m[1, 1], 0.0)
        np.testing.assert_array_almost_equal(m[2, 1], 1.0)


class TestCartesianTransformationOperator3DIFC4(test.bootstrap.IFC4):
    def test_identity_when_no_axes(self):
        op = self.file.createIfcCartesianTransformationOperator3D(
            LocalOrigin=self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))
        )
        m = subject.get_cartesiantransformationoperator3d(op)
        np.testing.assert_array_almost_equal(m, np.eye(4))

    def test_origin_translation(self):
        op = self.file.createIfcCartesianTransformationOperator3D(
            LocalOrigin=self.file.createIfcCartesianPoint((1.0, 2.0, 3.0))
        )
        m = subject.get_cartesiantransformationoperator3d(op)
        assert m[0, 3] == 1.0
        assert m[1, 3] == 2.0
        assert m[2, 3] == 3.0

    def test_uniform_scale(self):
        op = self.file.createIfcCartesianTransformationOperator3D(
            LocalOrigin=self.file.createIfcCartesianPoint((0.0, 0.0, 0.0)),
            Scale=2.0,
        )
        m = subject.get_cartesiantransformationoperator3d(op)
        assert m[0, 0] == 2.0
        assert m[1, 1] == 2.0
        assert m[2, 2] == 2.0
