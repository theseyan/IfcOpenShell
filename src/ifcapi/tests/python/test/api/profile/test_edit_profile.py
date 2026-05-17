# This file was generated with the assistance of an AI coding tool.

import pytest

import ifcopenshell.api.profile
import test.bootstrap


class TestEditProfile(test.bootstrap.IFC4):
    def test_edits_profile_attributes(self):
        profile = ifcopenshell.api.profile.add_parameterized_profile(self.file, ifc_class="IfcRectangleProfileDef")

        ifcopenshell.api.profile.edit_profile(
            self.file,
            profile=profile,
            attributes={"ProfileName": "100x200", "XDim": 100.0, "YDim": 200.0},
        )

        assert profile.ProfileName == "100x200"
        assert profile.XDim == 100.0
        assert profile.YDim == 200.0

    def test_rejects_unknown_attributes(self):
        profile = ifcopenshell.api.profile.add_parameterized_profile(self.file, ifc_class="IfcCircleProfileDef")

        with pytest.raises(RuntimeError, match="Attribute Missing not found"):
            ifcopenshell.api.profile.edit_profile(self.file, profile=profile, attributes={"Missing": 1})


class TestEditProfileIFC2X3(test.bootstrap.IFC2X3, TestEditProfile):
    pass
