# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import pytest

import test.bootstrap


class TestAddResource(test.bootstrap.IFC4):
    def test_adding_a_crew_resource(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        assert resource.is_a("IfcCrewResource")
        assert resource.Name == "Unnamed"

    def test_adding_a_named_resource(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource", name="My Labour"
        )
        assert resource.Name == "My Labour"

    def test_adding_a_child_resource(self):
        self.file.create_entity("IfcProject")
        crew = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        labour = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=crew, ifc_class="IfcLaborResource"
        )
        assert labour.is_a("IfcLaborResource")
        nests = self.file.by_type("IfcRelNests")
        assert len(nests) >= 1

    def test_reusing_a_declaration_and_preserving_sibling_order(self):
        project = self.file.create_entity("IfcProject")
        first = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        second = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        parent = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        child_a = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )
        child_b = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )

        declaration = self.file.by_type("IfcRelDeclares")[0]
        assert declaration.RelatingContext == project
        assert declaration.RelatedDefinitions == (first, second, parent)
        assert parent.IsNestedBy[0].RelatedObjects == (child_a, child_b)

    def test_preserving_upstream_partial_creation_without_a_context(self):
        with pytest.raises(RuntimeError, match="No IfcContext"):
            ifcopenshell.api.resource.add_resource(
                self.file, ifc_class="IfcCrewResource"
            )
        assert len(self.file.by_type("IfcCrewResource")) == 1


class TestAddResourceIFC2X3(test.bootstrap.IFC2X3, TestAddResource):
    def test_reusing_a_declaration_and_preserving_sibling_order(self):
        self.file.create_entity("IfcProject")
        parent = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        child_a = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )
        child_b = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent
        )
        assert parent.IsDecomposedBy[0].RelatedObjects == (child_a, child_b)

    def test_preserving_upstream_partial_creation_without_a_context(self):
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcCrewResource"
        )
        assert resource in self.file


class TestAddResourceIFC4X3(test.bootstrap.IFC4X3, TestAddResource):
    pass
