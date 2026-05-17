# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from math import cos, sin

import pytest

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.material
import ifcopenshell.api.root
import ifcopenshell.util.representation
import test.bootstrap


class WallRegenerationContext:
    def contexts(self, include_axis=True):
        self.file.createIfcProject()
        model = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        body = ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Body",
            target_view="MODEL_VIEW",
            parent=model,
        )
        axis = None
        if include_axis:
            plan = ifcopenshell.api.context.add_context(self.file, context_type="Plan")
            axis = ifcopenshell.api.context.add_context(
                self.file,
                context_type="Plan",
                context_identifier="Axis",
                target_view="GRAPH_VIEW",
                parent=plan,
            )
        return body, axis

    def layered_wall(self, body, axis=None, p1=(0.0, 0.0), p2=(4.0, 0.0), thickness=0.2, priority=0):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        material = ifcopenshell.api.material.add_material(self.file)
        layer_set = ifcopenshell.api.material.add_material_set(self.file, set_type="IfcMaterialLayerSet")
        layer = ifcopenshell.api.material.add_layer(self.file, layer_set=layer_set, material=material)
        ifcopenshell.api.material.edit_layer(
            self.file, layer=layer, attributes={"LayerThickness": thickness, "Priority": priority}
        )
        ifcopenshell.api.material.assign_material(
            self.file, products=[wall], type="IfcMaterialLayerSet", material=layer_set
        )
        body_rep = ifcopenshell.api.geometry.create_2pt_wall(
            self.file, wall, body, p1=p1, p2=p2, elevation=0.0, height=3.0, thickness=thickness
        )
        ifcopenshell.api.geometry.assign_representation(self.file, product=wall, representation=body_rep)
        if axis:
            length = ((p2[0] - p1[0]) ** 2 + (p2[1] - p1[1]) ** 2) ** 0.5
            axis_rep = ifcopenshell.api.geometry.add_axis_representation(
                self.file, axis, axis=[(0.0, 0.0), (length, 0.0)]
            )
            ifcopenshell.api.geometry.assign_representation(self.file, product=wall, representation=axis_rep)
        return wall


class TestRegenerateWallRepresentation(test.bootstrap.IFC4, WallRegenerationContext):
    def test_returns_none_for_walls_without_material_layers_but_creates_axis_context(self):
        self.contexts(include_axis=False)
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")

        assert ifcopenshell.api.geometry.regenerate_wall_representation(self.file, wall) is None
        assert ifcopenshell.util.representation.get_context(self.file, "Plan", "Axis", "GRAPH_VIEW") is not None

    def test_regenerates_unconnected_wall_from_layer_set_and_axis(self):
        body, axis = self.contexts()
        wall = self.layered_wall(body, axis)

        rep = ifcopenshell.api.geometry.regenerate_wall_representation(self.file, wall, length=1.0, height=1.0)
        item = rep.Items[0]
        curve = item.SweptArea.OuterCurve
        axis_rep = ifcopenshell.util.representation.get_representation(wall, "Plan", "Axis", "GRAPH_VIEW")

        assert rep.RepresentationType == "SweptSolid"
        assert item.Depth == 3.0
        assert curve.Points.CoordList == ((0.0, 0.0), (0.0, 0.2), (4.0, 0.2), (4.0, 0.0))
        assert axis_rep.Items[0].Points.CoordList == ((0.0, 0.0), (4.0, 0.0))

    def test_regenerates_connected_wall_and_reanchors_placement_to_trimmed_axis(self):
        body, axis = self.contexts()
        wall1 = self.layered_wall(body, axis, p1=(0.0, 0.0), p2=(4.0, 0.0))
        wall2 = self.layered_wall(body, axis, p1=(2.0, -1.0), p2=(2.0, 1.0))
        ifcopenshell.api.geometry.connect_wall(self.file, wall1, wall2)

        rep = ifcopenshell.api.geometry.regenerate_wall_representation(self.file, wall1, length=4.0, height=3.0)
        curve = rep.Items[0].SweptArea.OuterCurve
        axis_rep = ifcopenshell.util.representation.get_representation(wall1, "Plan", "Axis", "GRAPH_VIEW")

        assert len(curve.Points.CoordList) == 4
        for point, expected in zip(curve.Points.CoordList, ((0.0, 0.0), (-0.2, 0.2), (2.0, 0.2), (2.0, 0.0))):
            assert point == pytest.approx(expected)
        assert axis_rep.Items[0].Points.CoordList == ((0.0, 0.0), (2.0, 0.0))
        assert wall1.ObjectPlacement.RelativePlacement.Location.Coordinates == (2.0, 0.0, 0.0)

    def test_fallback_angle_creates_sloped_extrusion_when_no_body_exists(self):
        body, axis = self.contexts()
        wall = self.layered_wall(body, axis)
        ifcopenshell.api.geometry.unassign_representation(
            self.file,
            product=wall,
            representation=ifcopenshell.util.representation.get_representation(wall, "Model", "Body", "MODEL_VIEW"),
        )

        rep = ifcopenshell.api.geometry.regenerate_wall_representation(self.file, wall, length=4.0, height=3.0, angle=0.25)
        item = rep.Items[0]

        assert item.ExtrudedDirection.DirectionRatios == pytest.approx((0.0, sin(0.25), cos(0.25)))
        assert item.Depth == pytest.approx(3.0 / cos(0.25))

    def test_preserves_manual_booleans_without_reanchoring_placement(self):
        body, axis = self.contexts()
        wall = self.layered_wall(body, axis)
        ifcopenshell.api.geometry.unassign_representation(
            self.file,
            product=wall,
            representation=ifcopenshell.util.representation.get_representation(wall, "Plan", "Axis", "GRAPH_VIEW"),
        )
        axis_rep = ifcopenshell.api.geometry.add_axis_representation(self.file, axis, axis=[(1.0, 0.0), (4.0, 0.0)])
        ifcopenshell.api.geometry.assign_representation(self.file, product=wall, representation=axis_rep)
        body_rep = ifcopenshell.util.representation.get_representation(wall, "Model", "Body", "MODEL_VIEW")
        item = body_rep.Items[0]
        boolean = ifcopenshell.api.geometry.clip_solid(
            self.file, item, location=(1.0, 0.0, 0.0), normal=(1.0, 0.0, 0.0), element=wall
        )
        body_rep.Items = (boolean,)
        original_location = wall.ObjectPlacement.RelativePlacement.Location.Coordinates

        rep = ifcopenshell.api.geometry.regenerate_wall_representation(self.file, wall, length=3.0, height=3.0)
        axis_rep = ifcopenshell.util.representation.get_representation(wall, "Plan", "Axis", "GRAPH_VIEW")

        assert rep.Items[0] == boolean
        assert boolean.FirstOperand.is_a("IfcExtrudedAreaSolid")
        assert wall.ObjectPlacement.RelativePlacement.Location.Coordinates == original_location
        assert axis_rep.Items[0].Points.CoordList == ((1.0, 0.0), (4.0, 0.0))
