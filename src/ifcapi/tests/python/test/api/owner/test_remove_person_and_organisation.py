# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import ifcopenshell.guid
import test.bootstrap


class TestRemovePersonAndOrganisationIFC2X3(test.bootstrap.IFC2X3):
    def test_removing(self):
        user = self.file.createIfcPersonAndOrganization()
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert len(self.file.by_type("IfcPersonAndOrganization")) == 0

    def test_deleting_actors(self):
        user = self.file.createIfcPersonAndOrganization()
        self.file.createIfcActor(GlobalId=ifcopenshell.guid.new(), TheActor=user)
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert len(self.file.by_type("IfcActor")) == 0

    def test_ensuring_document_information_should_not_be_left_in_an_invalid_set_cardinality(self):
        user = self.file.createIfcPersonAndOrganization()
        document_information = self.file.createIfcDocumentInformation(Editors=[user])
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert document_information.Editors is None

    def test_deleting_owner_history(self):
        user = self.file.createIfcPersonAndOrganization()
        self.file.createIfcOwnerHistory(OwningUser=user)
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert len(self.file.by_type("IfcOwnerHistory")) == 0


class TestRemovePersonAndOrganisationIFC4(test.bootstrap.IFC4, TestRemovePersonAndOrganisationIFC2X3):
    def test_deleting_resource_approval_relationships(self):
        user = self.file.create_entity("IfcPersonAndOrganization")
        self.file.create_entity("IfcResourceApprovalRelationship", RelatedResourceObjects=[user])
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert len(self.file.by_type("IfcResourceApprovalRelationship")) == 0

    def test_deleting_resource_constraint_relationships(self):
        user = self.file.create_entity("IfcPersonAndOrganization")
        self.file.create_entity("IfcResourceConstraintRelationship", RelatedResourceObjects=[user])
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert len(self.file.by_type("IfcResourceConstraintRelationship")) == 0

    def test_deleting_external_reference_relationships(self):
        user = self.file.create_entity("IfcPersonAndOrganization")
        self.file.create_entity("IfcExternalReferenceRelationship", RelatedResourceObjects=[user])
        ifcopenshell.api.owner.remove_person_and_organisation(self.file, person_and_organisation=user)
        assert len(self.file.by_type("IfcExternalReferenceRelationship")) == 0
