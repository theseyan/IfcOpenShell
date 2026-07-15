# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2024 Dion Moult <dion@thinkmoult.com>
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

import ifcopenshell.api.control
import ifcopenshell.api.owner
import ifcopenshell.api.resource
import ifcopenshell.api.root
import ifcopenshell.api.sequence

import test.bootstrap


class TestRemoveResource(test.bootstrap.IFC4):
    def test_recursively_removing_resources_and_owned_relationships(self):
        self.file.create_entity("IfcProject")
        parent = ifcopenshell.api.resource.add_resource(self.file)
        child = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent, ifc_class="IfcLaborResource"
        )
        grandchild = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=child, ifc_class="IfcLaborResource"
        )
        sibling = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent, ifc_class="IfcLaborResource"
        )
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=child, related_object=product
        )
        ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=child, ifc_class="IfcQuantityTime"
        )
        ifcopenshell.api.resource.add_resource_time(self.file, resource=child)

        ids = {entity.id() for entity in (parent, child, grandchild, sibling)}
        ifcopenshell.api.resource.remove_resource(self.file, parent)

        assert not ids.intersection(
            entity.id() for entity in self.file.by_type("IfcConstructionResource")
        )
        assert product in self.file
        assert self.file.by_type("IfcRelNests") == []
        assert self.file.by_type("IfcRelAssignsToResource") == []
        assert self.file.by_type("IfcResourceTime") == []
        assert self.file.by_type("IfcPhysicalSimpleQuantity") == []

    def test_preserving_siblings_and_shared_declaration_order(self):
        self.file.create_entity("IfcProject")
        first = ifcopenshell.api.resource.add_resource(self.file)
        parent = ifcopenshell.api.resource.add_resource(self.file)
        child_a = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )
        child_b = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )
        last = ifcopenshell.api.resource.add_resource(self.file)
        child_a_id = child_a.id()
        child_b_id = child_b.id()

        ifcopenshell.api.resource.remove_resource(self.file, child_a)
        assert parent.IsNestedBy[0].RelatedObjects == (child_b,)
        ifcopenshell.api.resource.remove_resource(self.file, parent)

        declaration = self.file.by_type("IfcRelDeclares")[0]
        assert declaration.RelatedDefinitions == (first, last)
        remaining_ids = {entity.id() for entity in self.file}
        assert child_a_id not in remaining_ids
        assert child_b_id not in remaining_ids

    def test_removing_incoming_and_outgoing_resource_assignments(self):
        self.file.create_entity("IfcProject")
        relating = ifcopenshell.api.resource.add_resource(self.file)
        removed = ifcopenshell.api.resource.add_resource(self.file)
        other = ifcopenshell.api.resource.add_resource(self.file)
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        incoming = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=relating, related_object=removed
        )
        ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=relating, related_object=other
        )
        ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=removed, related_object=product
        )

        ifcopenshell.api.resource.remove_resource(self.file, removed)
        assert incoming.RelatedObjects == (other,)
        assert len(self.file.by_type("IfcRelAssignsToResource")) == 1
        assert relating in self.file and other in self.file and product in self.file

    def test_preserving_order_in_control_membership(self):
        self.file.create_entity("IfcProject")
        first = ifcopenshell.api.resource.add_resource(self.file)
        removed = ifcopenshell.api.resource.add_resource(self.file)
        last = ifcopenshell.api.resource.add_resource(self.file)
        task = ifcopenshell.api.sequence.add_task(self.file)
        relation = ifcopenshell.api.control.assign_control(
            self.file,
            relating_control=task,
            related_objects=[first, removed, last],
        )
        ifcopenshell.api.resource.remove_resource(self.file, removed)
        assert relation.RelatedObjects == (first, last)

    def test_preserving_a_shared_owner_history(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file)
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        history = self.file.create_entity("IfcOwnerHistory")
        resource.OwnerHistory = history
        wall.OwnerHistory = history
        ifcopenshell.api.resource.remove_resource(self.file, resource)
        assert wall.OwnerHistory == history
        assert history in self.file


class TestRemoveResourceIFC2X3(test.bootstrap.IFC2X3):
    def test_removing_nested_resources_without_declarations_or_usage(self):
        parent = ifcopenshell.api.resource.add_resource(self.file)
        child = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )
        ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=child, ifc_class="IfcQuantityTime"
        )
        ifcopenshell.api.resource.remove_resource(self.file, parent)
        assert self.file.by_type("IfcConstructionResource") == []
        assert self.file.by_type("IfcRelNests") == []
        assert self.file.by_type("IfcPhysicalSimpleQuantity") == []


class TestRemoveResourceIFC4X3(test.bootstrap.IFC4X3, TestRemoveResource):
    pass
