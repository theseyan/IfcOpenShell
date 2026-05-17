# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.root
import test.bootstrap


class WallConnectionContext:
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


class TestCreate2PtWall(test.bootstrap.IFC4, WallConnectionContext):
    def test_creates_wall_representation_and_updates_placement(self):
        context = self.body_context()
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        rep = ifcopenshell.api.geometry.create_2pt_wall(
            self.file, wall, context, p1=(2.0, 3.0), p2=(4.0, 3.0), elevation=0.5, height=3.0, thickness=0.2
        )

        assert rep.Items[0].Depth == 3.0
        assert wall.ObjectPlacement.RelativePlacement.Location.Coordinates == (2.0, 3.0, 0.5)


class TestConnectWall(test.bootstrap.IFC4, WallConnectionContext):
    def test_connects_intersecting_walls(self):
        context = self.body_context()
        wall1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        wall2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.geometry.create_2pt_wall(
            self.file, wall1, context, p1=(0.0, 0.0), p2=(4.0, 0.0), elevation=0.0, height=3.0, thickness=0.2
        )
        ifcopenshell.api.geometry.create_2pt_wall(
            self.file, wall2, context, p1=(2.0, -1.0), p2=(2.0, 1.0), elevation=0.0, height=3.0, thickness=0.2
        )

        connection = ifcopenshell.api.geometry.connect_wall(self.file, wall1, wall2)

        assert connection is not None
        assert connection.RelatingElement == wall1
        assert connection.RelatedElement == wall2
        assert connection.RelatingConnectionType == "ATEND"
        assert connection.RelatedConnectionType == "ATEND"

    def test_returns_none_for_parallel_walls(self):
        context = self.body_context()
        wall1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        wall2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.geometry.create_2pt_wall(
            self.file, wall1, context, p1=(0.0, 0.0), p2=(4.0, 0.0), elevation=0.0, height=3.0, thickness=0.2
        )
        ifcopenshell.api.geometry.create_2pt_wall(
            self.file, wall2, context, p1=(0.0, 1.0), p2=(4.0, 1.0), elevation=0.0, height=3.0, thickness=0.2
        )

        assert ifcopenshell.api.geometry.connect_wall(self.file, wall1, wall2) is None
