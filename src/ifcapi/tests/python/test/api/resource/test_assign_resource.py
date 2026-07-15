# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner.settings
import ifcopenshell.api.resource
import ifcopenshell.api.root

import test.bootstrap


class TestAssignResource(test.bootstrap.IFC4):
    def test_assigning_a_resource_to_a_product(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcConstructionEquipmentResource"
        )
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        rel = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product
        )
        assert rel.is_a("IfcRelAssignsToResource")
        assert rel.RelatingResource == resource
        assert product in rel.RelatedObjects

    def test_appending_to_existing_assignment(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcConstructionEquipmentResource"
        )
        product1 = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        product2 = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product1
        )
        rel = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product2
        )
        assert len(rel.RelatedObjects) == 2

    def test_repeated_pair_is_idempotent(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcConstructionEquipmentResource"
        )
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcBuildingElementProxy"
        )
        first = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product
        )
        second = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=product
        )
        assert second == first
        assert first.RelatedObjects == (product,)

    def test_assigning_an_actor(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file)
        actor = self.file.create_entity("IfcActor")
        relation = ifcopenshell.api.resource.assign_resource(
            self.file, relating_resource=resource, related_object=actor
        )
        assert relation.RelatedObjects == (actor,)

    def test_creating_and_updating_owner_history(self):
        user = self.file.createIfcPersonAndOrganization()
        application = self.file.createIfcApplication()
        old_user = ifcopenshell.api.owner.settings.get_user
        old_application = ifcopenshell.api.owner.settings.get_application
        ifcopenshell.api.owner.settings.get_user = lambda file: user
        ifcopenshell.api.owner.settings.get_application = lambda file: application
        try:
            self.file.create_entity("IfcProject")
            resource = ifcopenshell.api.resource.add_resource(self.file)
            first = ifcopenshell.api.root.create_entity(
                self.file, ifc_class="IfcBuildingElementProxy"
            )
            second = ifcopenshell.api.root.create_entity(
                self.file, ifc_class="IfcBuildingElementProxy"
            )
            relation = ifcopenshell.api.resource.assign_resource(
                self.file, relating_resource=resource, related_object=first
            )
            assert relation.OwnerHistory.ChangeAction == "ADDED"
            ifcopenshell.api.resource.assign_resource(
                self.file, relating_resource=resource, related_object=second
            )
            assert relation.OwnerHistory.ChangeAction == "MODIFIED"
            assert relation.OwnerHistory.LastModifyingUser == user
            assert relation.OwnerHistory.LastModifyingApplication == application
        finally:
            ifcopenshell.api.owner.settings.get_user = old_user
            ifcopenshell.api.owner.settings.get_application = old_application
