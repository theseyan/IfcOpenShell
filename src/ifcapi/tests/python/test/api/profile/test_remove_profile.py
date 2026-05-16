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

import ifcopenshell.api.profile
import ifcopenshell.api.pset
import test.bootstrap


class TestRemoveProfileIFC2X3(test.bootstrap.IFC2X3):
    def test_removing_profile(self):
        profile = ifcopenshell.api.profile.add_parameterized_profile(self.file, "IfcRectangleProfileDef")
        ifcopenshell.api.profile.remove_profile(self.file, profile=profile)
        assert len(self.file.by_type("IfcRectangleProfileDef")) == 0

    def test_removing_profile_with_pset(self):
        profile = ifcopenshell.api.profile.add_parameterized_profile(self.file, "IfcRectangleProfileDef")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=profile, name="ProfilePset")
        ifcopenshell.api.profile.remove_profile(self.file, profile=profile)
        assert len(self.file.by_type("IfcRectangleProfileDef")) == 0
        assert len(self.file.by_type("IfcProfileProperties")) == 0


class TestRemoveProfileIFC4(test.bootstrap.IFC4, TestRemoveProfileIFC2X3):
    def test_not_removing_aggregate_subelements(self):
        outer = self.file.createIfcIndexedPolyCurve(
            self.file.createIfcCartesianPointList2D(((0.0, 0.0), (1.0, 0.0), (1.0, 1.0), (0.0, 0.0)))
        )
        inner = self.file.createIfcIndexedPolyCurve(
            self.file.createIfcCartesianPointList2D(((0.25, 0.25), (0.5, 0.25), (0.25, 0.5), (0.25, 0.25)))
        )
        profile = self.file.createIfcArbitraryProfileDefWithVoids("AREA", None, outer, [inner])

        ifcopenshell.api.profile.remove_profile(self.file, profile=profile)

        assert len(self.file.by_type("IfcArbitraryProfileDefWithVoids")) == 0
        assert len(self.file.by_type("IfcIndexedPolyCurve")) == 1
        assert self.file.by_type("IfcIndexedPolyCurve")[0] == inner
