# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.root
import ifcopenshell.api.structural
import test.bootstrap


def assert_removed(file, entity_id):
    with pytest.raises(RuntimeError, match=f"Entity #{entity_id} not found"):
        file.by_id(entity_id)


class TestAddStructuralBoundaryCondition(test.bootstrap.IFC4):
    def test_adds_orphan_boundary_condition_from_requested_class(self):
        condition = ifcopenshell.api.structural.add_structural_boundary_condition(
            self.file, name="Pinned", ifc_class="IfcBoundaryFaceCondition"
        )

        assert condition.is_a("IfcBoundaryFaceCondition")
        assert condition.Name == "Pinned"

    def test_adds_boundary_condition_to_point_connection(self):
        connection = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralPointConnection")

        condition = ifcopenshell.api.structural.add_structural_boundary_condition(
            self.file, name="Node", connection=connection
        )

        assert condition.is_a("IfcBoundaryNodeCondition")
        assert condition.Name == "Node"
        assert connection.AppliedCondition == condition

    def test_adds_boundary_condition_to_member_connection_relation(self):
        member = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralCurveMember")
        connection = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralCurveConnection")
        relation = ifcopenshell.api.structural.add_structural_member_connection(
            self.file, relating_structural_member=member, related_structural_connection=connection
        )

        condition = ifcopenshell.api.structural.add_structural_boundary_condition(
            self.file, name="Edge", connection=relation
        )

        assert condition.is_a("IfcBoundaryEdgeCondition")
        assert relation.AppliedCondition == condition
        assert connection.AppliedCondition is None

    def test_rejects_member_connection_relation_without_related_connection(self):
        relation = self.file.create_entity("IfcRelConnectsStructuralMember")

        with pytest.raises(RuntimeError, match="RelatedStructuralConnection is null or missing"):
            ifcopenshell.api.structural.add_structural_boundary_condition(self.file, connection=relation)


class TestRemoveStructuralBoundaryCondition(test.bootstrap.IFC4):
    def test_removes_condition_from_connection_and_deletes_unshared_condition(self):
        connection = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralPointConnection")
        condition = ifcopenshell.api.structural.add_structural_boundary_condition(self.file, connection=connection)
        condition_id = condition.id()

        ifcopenshell.api.structural.remove_structural_boundary_condition(self.file, connection=connection)

        assert connection.AppliedCondition is None
        assert_removed(self.file, condition_id)

    def test_unassigns_orphan_condition_from_all_connections_before_removal(self):
        condition = ifcopenshell.api.structural.add_structural_boundary_condition(self.file)
        condition_id = condition.id()
        connection1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralPointConnection")
        connection2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralPointConnection")
        connection1.AppliedCondition = condition
        connection2.AppliedCondition = condition

        ifcopenshell.api.structural.remove_structural_boundary_condition(
            self.file, boundary_condition=condition
        )

        assert connection1.AppliedCondition is None
        assert connection2.AppliedCondition is None
        assert_removed(self.file, condition_id)

    def test_requires_a_connection_or_boundary_condition(self):
        with pytest.raises(AssertionError):
            ifcopenshell.api.structural.remove_structural_boundary_condition(self.file)


class TestRemoveStructuralConnectionCondition(test.bootstrap.IFC4):
    def test_removes_relation_and_preserves_relation_condition_as_upstream_orphan(self):
        member = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralCurveMember")
        connection = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralPointConnection")
        relation = ifcopenshell.api.structural.add_structural_member_connection(
            self.file, relating_structural_member=member, related_structural_connection=connection
        )
        condition = ifcopenshell.api.structural.add_structural_boundary_condition(
            self.file, connection=relation
        )
        relation_id = relation.id()
        condition_id = condition.id()

        ifcopenshell.api.structural.remove_structural_connection_condition(self.file, relation=relation)

        assert_removed(self.file, relation_id)
        assert self.file.by_id(condition_id).is_a("IfcBoundaryNodeCondition")
        assert member in self.file
        assert connection in self.file


class TestEditStructuralConnectionCs(test.bootstrap.IFC4):
    def test_creates_and_replaces_connection_coordinate_system_directions(self):
        relation = self.file.create_entity("IfcRelConnectsStructuralMember")

        ifcopenshell.api.structural.edit_structural_connection_cs(
            self.file, structural_item=relation, axis=(0.0, 1.0, 0.0), ref_direction=(0.0, 0.0, 1.0)
        )
        ccs = relation.ConditionCoordinateSystem
        old_axis = ccs.Axis
        old_ref_direction = ccs.RefDirection
        old_axis_id = old_axis.id()
        old_ref_direction_id = old_ref_direction.id()

        assert ccs.Location.Coordinates == (0.0, 0.0, 0.0)
        assert old_axis.DirectionRatios == (0.0, 1.0, 0.0)
        assert old_ref_direction.DirectionRatios == (0.0, 0.0, 1.0)

        ifcopenshell.api.structural.edit_structural_connection_cs(
            self.file, structural_item=relation, axis=(1.0, 0.0, 0.0), ref_direction=(0.0, 1.0, 0.0)
        )

        assert ccs.Axis.DirectionRatios == (1.0, 0.0, 0.0)
        assert ccs.RefDirection.DirectionRatios == (0.0, 1.0, 0.0)
        assert_removed(self.file, old_axis_id)
        assert_removed(self.file, old_ref_direction_id)

    def test_rejects_entities_without_connection_coordinate_system_attribute_without_creating_orphans(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")

        with pytest.raises(RuntimeError, match="ConditionCoordinateSystem"):
            ifcopenshell.api.structural.edit_structural_connection_cs(self.file, structural_item=wall)

        assert self.file.by_type("IfcAxis2Placement3D") == []
        assert self.file.by_type("IfcCartesianPoint") == []
        assert self.file.by_type("IfcDirection") == []


class TestEditStructuralItemAxis(test.bootstrap.IFC4):
    def test_replaces_structural_item_axis(self):
        member = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcStructuralCurveMember")
        member.Axis = self.file.create_entity("IfcDirection", (0.0, 0.0, 1.0))
        old_axis = member.Axis
        old_axis_id = old_axis.id()

        ifcopenshell.api.structural.edit_structural_item_axis(self.file, structural_item=member, axis=(0.0, 1.0, 0.0))

        assert member.Axis.DirectionRatios == (0.0, 1.0, 0.0)
        assert_removed(self.file, old_axis_id)

    def test_rejects_entities_without_axis_attribute_without_creating_orphans(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")

        with pytest.raises(RuntimeError, match="Axis attribute"):
            ifcopenshell.api.structural.edit_structural_item_axis(self.file, structural_item=wall)

        assert self.file.by_type("IfcDirection") == []
