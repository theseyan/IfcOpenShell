# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2025 Dion Moult <dion@thinkmoult.com>
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

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.root
import ifcopenshell.api.unit
import pytest
import test.bootstrap
from ifcopenshell.util.data import Clipping
from ifcopenshell.util.shape_builder import ShapeBuilder


class TestAddProfileRepresentation(test.bootstrap.IFC4):
    def setup_profile(self, prefix=None) -> None:
        builder = ShapeBuilder(self.file)

        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        # In IFC2X3 the unit is required.
        unit = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix=prefix)
        ifcopenshell.api.unit.assign_unit(self.file, [unit])

        model_context = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        self.body = ifcopenshell.api.context.add_context(
            self.file, context_type="Model", context_identifier="Body", target_view="MODEL_VIEW", parent=model_context
        )
        rectangle = builder.rectangle((100, 100))
        self.profile = builder.profile(rectangle)

    def test_run(self):
        self.setup_profile()
        representation_string_cardinal_point = ifcopenshell.api.geometry.add_profile_representation(
            self.file,
            context=self.body,
            profile=self.profile,
            depth=1000,
            cardinal_point="bottom left",
        )
        representation_numeric_cardinal_point = ifcopenshell.api.geometry.add_profile_representation(
            self.file,
            context=self.body,
            profile=self.profile,
            depth=1000,
            cardinal_point=1,
        )
        representations = [representation_string_cardinal_point, representation_numeric_cardinal_point]
        for representation in representations:
            assert representation.is_a("IfcShapeRepresentation")
            item = representation.Items[0]
            assert item.is_a("IfcExtrudedAreaSolid")
            assert item.SweptArea == self.profile
            assert item.ExtrudedDirection.DirectionRatios == (0.0, 0.0, 1.0)
            assert item.Depth == 1000
            assert item.Position.Location.Coordinates == (-50.0, 50.0, 0.0)

    def test_cardinal_points(self):
        self.setup_profile()
        cardinal_points = {
            "bottom left": (-50.0, 50.0, 0.0),
            "bottom centre": (0.0, 50.0, 0.0),
            "bottom right": (50.0, 50.0, 0.0),
            "mid-depth left": (-50.0, 0.0, 0.0),
            "mid-depth centre": (0.0, 0.0, 0.0),
            "mid-depth right": (50.0, 0.0, 0.0),
            "top left": (-50.0, -50.0, 0.0),
            "top centre": (0.0, -50.0, 0.0),
            "top right": (50.0, -50.0, 0.0),
        }
        for index, (cardinal_point, expected) in enumerate(cardinal_points.items(), 1):
            for choice in (cardinal_point, index):
                representation = ifcopenshell.api.geometry.add_profile_representation(
                    self.file, self.body, self.profile, cardinal_point=choice
                )
                assert representation.Items[0].Position.Location.Coordinates == expected

    def test_default_none_and_fallback_cardinal_points(self):
        self.setup_profile()
        for cardinal_point in (None, "geometric centroid", 10, 19):
            representation = ifcopenshell.api.geometry.add_profile_representation(
                self.file, self.body, self.profile, cardinal_point=cardinal_point
            )
            assert representation.Items[0].Position.Location.Coordinates == (0.0, 0.0, 0.0)

        default_representation = ifcopenshell.api.geometry.add_profile_representation(self.file, self.body, self.profile)
        assert default_representation.Items[0].Position.Location.Coordinates == (0.0, 0.0, 0.0)
        with pytest.raises(IndexError):
            ifcopenshell.api.geometry.add_profile_representation(self.file, self.body, self.profile, cardinal_point=20)

    def test_axes_and_representation_type(self):
        self.setup_profile()
        representation = ifcopenshell.api.geometry.add_profile_representation(
            self.file,
            self.body,
            self.profile,
            cardinal_point=None,
            placement_zx_axes=((0.0, 1.0, 0.0), (0.0, 0.0, -1.0)),
        )
        placement = representation.Items[0].Position
        assert placement.Axis.DirectionRatios == (0.0, 1.0, 0.0)
        assert placement.RefDirection.DirectionRatios == (0.0, 0.0, -1.0)
        assert representation.RepresentationType == "SweptSolid"

        default_representation = ifcopenshell.api.geometry.add_profile_representation(
            self.file, self.body, self.profile, cardinal_point=None
        )
        default_placement = default_representation.Items[0].Position
        assert default_placement.Axis.DirectionRatios == (0.0, 0.0, 1.0)
        assert default_placement.RefDirection.DirectionRatios == (1.0, 0.0, 0.0)

    def test_units_plane_clipping_and_input_list(self):
        self.setup_profile(prefix="MILLI")
        clipping = Clipping(location=(1.0, 2.0, 3.0), normal=(0.0, 0.0, 1.0))
        clippings = [clipping]
        representation = ifcopenshell.api.geometry.add_profile_representation(
            self.file, self.body, self.profile, depth=1.0, cardinal_point=None, clippings=clippings
        )
        item = representation.Items[0]
        assert item.is_a("IfcBooleanClippingResult")
        assert item.FirstOperand.Depth == 1000.0
        assert item.SecondOperand.BaseSurface.Position.Location.Coordinates == (1000.0, 2000.0, 3000.0)
        assert representation.RepresentationType == "Clipping"
        assert clippings == [clipping]

    def test_mixed_clipping_order_copies_entities_and_preserves_inputs(self):
        self.setup_profile()
        source = ifcopenshell.api.geometry.add_profile_representation(
            self.file, self.body, self.profile, cardinal_point=None
        ).Items[0]
        pre_existing = ifcopenshell.api.geometry.clip_solid(
            self.file, source, location=(20.0, 0.0, 0.0), normal=(1.0, 0.0, 0.0)
        )
        original_first_operand = pre_existing.FirstOperand
        first_plane = Clipping(location=(1.0, 0.0, 0.0), normal=(1.0, 0.0, 0.0))
        last_plane = Clipping(location=(3.0, 0.0, 0.0), normal=(1.0, 0.0, 0.0))
        clippings = [first_plane, pre_existing, last_plane]
        representation = ifcopenshell.api.geometry.add_profile_representation(
            self.file, self.body, self.profile, cardinal_point=None, clippings=clippings
        )

        outer = representation.Items[0]
        copied_entity = outer.FirstOperand
        innermost_plane = copied_entity.FirstOperand
        assert outer.is_a("IfcBooleanClippingResult")
        assert copied_entity.is_a("IfcBooleanClippingResult")
        assert innermost_plane.is_a("IfcBooleanClippingResult")
        assert outer.SecondOperand.BaseSurface.Position.Location.Coordinates == (1.0, 0.0, 0.0)
        assert innermost_plane.SecondOperand.BaseSurface.Position.Location.Coordinates == (3.0, 0.0, 0.0)
        assert copied_entity.id() != pre_existing.id()
        assert pre_existing.FirstOperand == original_first_operand
        assert clippings == [first_plane, pre_existing, last_plane]


class TestAddProfileRepresentationIFC2X3(test.bootstrap.IFC2X3, TestAddProfileRepresentation):
    pass


class TestAddProfileRepresentationIFC4X3(test.bootstrap.IFC4X3, TestAddProfileRepresentation):
    pass
