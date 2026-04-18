# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import test.bootstrap


class TestAddResource(test.bootstrap.IFC4):
    def test_adding_a_crew_resource(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcCrewResource")
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
        crew = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcCrewResource")
        labour = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=crew, ifc_class="IfcLaborResource"
        )
        assert labour.is_a("IfcLaborResource")
        nests = self.file.by_type("IfcRelNests")
        assert len(nests) >= 1


class TestAddResourceIFC2X3(test.bootstrap.IFC2X3, TestAddResource):
    pass
