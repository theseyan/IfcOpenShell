# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.profile
import test.bootstrap


class TestAddArbitraryProfileWithVoids(test.bootstrap.IFC4):
    def test_creates_indexed_polycurve_profile_with_voids(self):
        profile = ifcopenshell.api.profile.add_arbitrary_profile_with_voids(
            self.file,
            outer_profile=[(0.0, 0.0), (0.4, 0.0), (0.4, 0.4), (0.0, 0.4), (0.0, 0.0)],
            inner_profiles=[[(0.1, 0.1), (0.3, 0.1), (0.3, 0.3), (0.1, 0.3), (0.1, 0.1)]],
            name="SK01 Hole Profile",
        )
        outer_curve = profile.OuterCurve
        inner_curve = profile.InnerCurves[0]
        assert profile.is_a("IfcArbitraryProfileDefWithVoids")
        assert profile.ProfileType == "AREA"
        assert profile.ProfileName == "SK01 Hole Profile"
        assert outer_curve.is_a("IfcIndexedPolyCurve")
        assert outer_curve.Points.is_a("IfcCartesianPointList3D")
        assert outer_curve.Points.CoordList == ((0.0, 0.0), (0.4, 0.0), (0.4, 0.4), (0.0, 0.4), (0.0, 0.0))
        assert inner_curve.is_a("IfcIndexedPolyCurve")
        assert inner_curve.Points.is_a("IfcCartesianPointList2D")
        assert inner_curve.Points.CoordList == ((0.1, 0.1), (0.3, 0.1), (0.3, 0.3), (0.1, 0.3), (0.1, 0.1))


class TestAddArbitraryProfileWithVoidsIFC2X3(test.bootstrap.IFC2X3, TestAddArbitraryProfileWithVoids):
    def test_creates_indexed_polycurve_profile_with_voids(self):
        profile = ifcopenshell.api.profile.add_arbitrary_profile_with_voids(
            self.file,
            outer_profile=[(0.0, 0.0), (0.4, 0.0), (0.4, 0.4), (0.0, 0.4), (0.0, 0.0)],
            inner_profiles=[[(0.1, 0.1), (0.3, 0.1), (0.3, 0.3), (0.1, 0.3), (0.1, 0.1)]],
            name="SK01 Hole Profile",
        )
        outer_curve = profile.OuterCurve
        inner_curve = profile.InnerCurves[0]
        assert profile.ProfileName == "SK01 Hole Profile"
        assert outer_curve.is_a("IfcPolyline")
        assert inner_curve.is_a("IfcPolyline")
        assert [point.Coordinates for point in outer_curve.Points] == [
            (0.0, 0.0),
            (0.4, 0.0),
            (0.4, 0.4),
            (0.0, 0.4),
            (0.0, 0.0),
        ]
        assert [point.Coordinates for point in inner_curve.Points] == [
            (0.1, 0.1),
            (0.3, 0.1),
            (0.3, 0.3),
            (0.1, 0.3),
            (0.1, 0.1),
        ]
