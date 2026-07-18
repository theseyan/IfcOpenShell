# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from math import cos, sin

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import pytest
from ifcopenshell.util.data import Clipping

import test.bootstrap


class WallSlabContext:
    def body_context(self):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        return ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Body",
            target_view="MODEL_VIEW",
            parent=parent,
        )


class TestAddWallRepresentation(test.bootstrap.IFC4, WallSlabContext):
    def test_creates_default_wall_body(self):
        context = self.body_context()
        rep = ifcopenshell.api.geometry.add_wall_representation(self.file, context)
        item = rep.Items[0]
        curve = item.SweptArea.OuterCurve

        assert rep.ContextOfItems == context
        assert rep.RepresentationIdentifier == "Body"
        assert rep.RepresentationType == "SweptSolid"
        assert curve.is_a("IfcIndexedPolyCurve")
        assert curve.SelfIntersect is False
        assert curve.Points.CoordList == (
            (0.0, 0.0),
            (0.0, 0.2),
            (1.0, 0.2),
            (1.0, 0.0),
            (0.0, 0.0),
        )
        assert item.Position.Location.Coordinates == (0.0, 0.0, 0.0)
        assert item.ExtrudedDirection.DirectionRatios == (0.0, 0.0, 1.0)
        assert item.Depth == 3.0

    def test_negative_wall_flips_thickness_not_extrusion_direction(self):
        context = self.body_context()
        rep = ifcopenshell.api.geometry.add_wall_representation(
            self.file,
            context,
            direction_sense="NEGATIVE",
            x_angle=0.25,
            thickness=0.2,
            height=3.0,
            offset=0.4,
        )
        item = rep.Items[0]
        points = item.SweptArea.OuterCurve.Points.CoordList

        assert points[1][1] == pytest.approx(-0.2 / cos(0.25))
        assert item.ExtrudedDirection.DirectionRatios == pytest.approx(
            (0.0, sin(0.25), cos(0.25))
        )
        assert item.Depth == pytest.approx(3.0 * abs(1 / cos(0.25)))
        assert item.Position.Location.Coordinates == (0.0, 0.4, 0.0)

    def test_booleans_are_applied_before_clippings_and_original_boolean_list_is_drained(
        self,
    ):
        context = self.body_context()
        clipping = Clipping(location=(0.0, 0.0, 1.0), normal=(0.0, 0.0, 1.0))
        boolean = self.file.createIfcBooleanResult(
            "DIFFERENCE",
            self.file.createIfcBlock(
                self.file.createIfcAxis2Placement3D(
                    self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))
                ),
                1.0,
                1.0,
                1.0,
            ),
            self.file.createIfcBlock(
                self.file.createIfcAxis2Placement3D(
                    self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))
                ),
                0.5,
                0.5,
                0.5,
            ),
        )
        booleans = [boolean]
        rep = ifcopenshell.api.geometry.add_wall_representation(
            self.file, context, booleans=booleans, clippings=[clipping]
        )
        item = rep.Items[0]

        assert booleans == []
        assert rep.RepresentationType == "Clipping"
        assert item.is_a("IfcBooleanClippingResult")
        assert item.FirstOperand == boolean
        assert boolean.FirstOperand.is_a("IfcExtrudedAreaSolid")

    def test_rejects_unknown_direction_without_mutation(self):
        context = self.body_context()
        before = len(list(self.file))
        with pytest.raises(
            ValueError, match="Unsupported GeometryDirectionSense value"
        ):
            ifcopenshell.api.geometry.add_wall_representation(
                self.file, context, direction_sense="positive"
            )
        assert len(list(self.file)) == before


class TestAddWallRepresentationIFC2X3(test.bootstrap.IFC2X3, WallSlabContext):
    def test_creates_polyline_wall_body(self):
        context = self.body_context()
        rep = ifcopenshell.api.geometry.add_wall_representation(self.file, context)
        curve = rep.Items[0].SweptArea.OuterCurve

        assert curve.is_a("IfcPolyline")
        assert [point.Coordinates for point in curve.Points] == [
            (0.0, 0.0),
            (0.0, 0.2),
            (1.0, 0.2),
            (1.0, 0.0),
            (0.0, 0.0),
        ]


class TestAddSlabRepresentation(test.bootstrap.IFC4, WallSlabContext):
    def test_default_ifc4_slab_has_no_position_and_no_self_intersect_value(self):
        context = self.body_context()
        rep = ifcopenshell.api.geometry.add_slab_representation(self.file, context)
        item = rep.Items[0]
        curve = item.SweptArea.OuterCurve

        assert rep.RepresentationType == "SweptSolid"
        assert item.Position is None
        assert curve.is_a("IfcIndexedPolyCurve")
        assert curve.SelfIntersect is None
        assert curve.Points.CoordList == (
            (0.0, 0.0),
            (1.0, 0.0),
            (1.0, 1.0),
            (0.0, 1.0),
            (0.0, 0.0),
        )

    def test_slab_polyline_slope_and_negative_direction(self):
        context = self.body_context()
        rep = ifcopenshell.api.geometry.add_slab_representation(
            self.file,
            context,
            depth=0.3,
            direction_sense="NEGATIVE",
            offset=0.2,
            x_angle=0.25,
            polyline=[(0.0, 0.0), (1.0, 1.0), (0.0, 1.0), (0.0, 0.0)],
        )
        item = rep.Items[0]

        assert item.SweptArea.OuterCurve.Points.CoordList[1] == pytest.approx(
            (1.0, abs(1 / cos(0.25)))
        )
        assert item.ExtrudedDirection.DirectionRatios == pytest.approx(
            (0.0, -sin(0.25), -cos(0.25))
        )
        assert item.Position.Location.Coordinates == pytest.approx(
            (
                0.0,
                sin(0.25) * 0.2 * abs(1 / cos(0.25)),
                cos(0.25) * 0.2 * abs(1 / cos(0.25)),
            )
        )
        assert item.Depth == pytest.approx(0.3 * abs(1 / cos(0.25)))

    def test_slab_clipping_list_is_drained_like_upstream(self):
        context = self.body_context()
        clippings = [Clipping(location=(0.0, 0.0, 1.0), normal=(0.0, 0.0, 1.0))]
        rep = ifcopenshell.api.geometry.add_slab_representation(
            self.file, context, clippings=clippings
        )

        assert clippings == []
        assert rep.RepresentationType == "Clipping"
        assert rep.Items[0].is_a("IfcBooleanClippingResult")

    def test_rejects_unknown_direction_without_mutation(self):
        context = self.body_context()
        before = len(list(self.file))
        with pytest.raises(
            ValueError, match="Unsupported GeometryDirectionSense value"
        ):
            ifcopenshell.api.geometry.add_slab_representation(
                self.file, context, direction_sense="positive"
            )
        assert len(list(self.file)) == before


class TestAddSlabRepresentationIFC2X3(test.bootstrap.IFC2X3, WallSlabContext):
    def test_ifc2x3_slab_has_default_position(self):
        context = self.body_context()
        rep = ifcopenshell.api.geometry.add_slab_representation(self.file, context)
        item = rep.Items[0]

        assert item.Position is not None
        assert item.SweptArea.OuterCurve.is_a("IfcPolyline")
