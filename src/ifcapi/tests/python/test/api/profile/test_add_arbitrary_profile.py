# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.profile
import test.bootstrap


class TestAddArbitraryProfile(test.bootstrap.IFC4):
    def test_creates_indexed_polycurve_profile(self):
        profile = ifcopenshell.api.profile.add_arbitrary_profile(
            self.file,
            profile=[(0.0, 0.0), (0.01, 0.0), (0.01, 0.1), (0.0, 0.1), (0.0, 0.0)],
            name="SK01 Profile",
        )
        curve = profile.OuterCurve
        assert profile.is_a("IfcArbitraryClosedProfileDef")
        assert profile.ProfileType == "AREA"
        assert profile.ProfileName == "SK01 Profile"
        assert curve.is_a("IfcIndexedPolyCurve")
        assert curve.Points.is_a("IfcCartesianPointList2D")
        assert curve.Points.CoordList == ((0.0, 0.0), (0.01, 0.0), (0.01, 0.1), (0.0, 0.1), (0.0, 0.0))

    def test_invalid_dimensions_are_an_error(self):
        with pytest.raises(RuntimeError):
            ifcopenshell.api.profile.add_arbitrary_profile(self.file, profile=[(0.0,), (1.0,)])


class TestAddArbitraryProfileIFC2X3(test.bootstrap.IFC2X3, TestAddArbitraryProfile):
    def test_creates_indexed_polycurve_profile(self):
        profile = ifcopenshell.api.profile.add_arbitrary_profile(
            self.file,
            profile=[(0.0, 0.0), (0.01, 0.0), (0.01, 0.1), (0.0, 0.1), (0.0, 0.0)],
            name="SK01 Profile",
        )
        curve = profile.OuterCurve
        assert profile.ProfileName == "SK01 Profile"
        assert curve.is_a("IfcPolyline")
        assert [point.Coordinates for point in curve.Points] == [
            (0.0, 0.0),
            (0.01, 0.0),
            (0.01, 0.1),
            (0.0, 0.1),
            (0.0, 0.0),
        ]
