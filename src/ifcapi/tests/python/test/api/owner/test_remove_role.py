# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestRemoveRoleIFC2X3(test.bootstrap.IFC2X3):
    def test_removing_a_role(self):
        role = self.file.createIfcActorRole()
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert len(self.file.by_type("IfcActorRole")) == 0

    def test_ensuring_organisation_cardinality_is_valid(self):
        role = self.file.createIfcActorRole()
        organisation = self.file.createIfcOrganization()
        organisation.Roles = [role]
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert organisation.Roles is None

    def test_ensuring_person_cardinality_is_valid(self):
        role = self.file.createIfcActorRole()
        person = self.file.createIfcPerson()
        person.Roles = [role]
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert person.Roles is None

    def test_ensuring_person_and_organisation_cardinality_is_valid(self):
        role = self.file.createIfcActorRole()
        person_and_organisation = self.file.createIfcPersonAndOrganization()
        person_and_organisation.Roles = [role]
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert person_and_organisation.Roles is None


class TestRemoveRoleIFC4(test.bootstrap.IFC4, TestRemoveRoleIFC2X3):
    def test_deleting_resource_approval_relationships(self):
        role = self.file.create_entity("IfcActorRole")
        self.file.create_entity("IfcResourceApprovalRelationship", RelatedResourceObjects=[role])
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert len(self.file.by_type("IfcResourceApprovalRelationship")) == 0

    def test_deleting_resource_constraint_relationships(self):
        role = self.file.create_entity("IfcActorRole")
        self.file.create_entity("IfcResourceConstraintRelationship", RelatedResourceObjects=[role])
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert len(self.file.by_type("IfcResourceConstraintRelationship")) == 0

    def test_deleting_external_reference_relationships(self):
        role = self.file.create_entity("IfcActorRole")
        self.file.create_entity("IfcExternalReferenceRelationship", RelatedResourceObjects=[role])
        ifcopenshell.api.owner.remove_role(self.file, role=role)
        assert len(self.file.by_type("IfcExternalReferenceRelationship")) == 0
