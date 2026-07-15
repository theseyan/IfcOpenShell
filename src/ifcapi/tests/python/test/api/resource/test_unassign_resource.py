# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import ifcopenshell.api.root

import test.bootstrap


class TestUnassignResource(test.bootstrap.IFC4):
    def test_unassigning_a_resource(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcConstructionEquipmentResource"
        )
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product
        )
        assert len(self.file.by_type("IfcRelAssignsToResource")) == 1
        ifcopenshell.api.resource.unassign_resource(
            self.file, relating_resource=resource, related_object=product
        )
        assert len(self.file.by_type("IfcRelAssignsToResource")) == 0

    def test_preserving_order_and_other_resources(self):
        self.file.create_entity("IfcProject")
        first_resource = ifcopenshell.api.resource.add_resource(self.file)
        second_resource = ifcopenshell.api.resource.add_resource(self.file)
        first = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        middle = self.file.create_entity("IfcActor")
        last = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        for related in (first, middle, last):
            relation = ifcopenshell.api.resource.assign_resource(
                self.file, relating_resource=first_resource, related_object=related
            )
        other = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=second_resource, related_object=middle
        )

        ifcopenshell.api.resource.unassign_resource(
            self.file, relating_resource=first_resource, related_object=middle
        )
        assert relation.RelatedObjects == (first, last)
        assert other.RelatedObjects == (middle,)
        ifcopenshell.api.resource.unassign_resource(
            self.file, relating_resource=first_resource, related_object=middle
        )
        assert relation.RelatedObjects == (first, last)
