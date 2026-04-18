# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import ifcopenshell.api.root
import test.bootstrap


class TestAssignResource(test.bootstrap.IFC4):
    def test_assigning_a_resource_to_a_product(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcConstructionEquipmentResource")
        product = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuildingElementProxy")
        rel = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product
        )
        assert rel.is_a("IfcRelAssignsToResource")
        assert rel.RelatingResource == resource
        assert product in rel.RelatedObjects

    def test_appending_to_existing_assignment(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcConstructionEquipmentResource")
        product1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuildingElementProxy")
        product2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuildingElementProxy")
        ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product1
        )
        rel = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product2
        )
        assert len(rel.RelatedObjects) == 2
