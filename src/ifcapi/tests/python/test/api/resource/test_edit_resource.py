# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import test.bootstrap


class TestEditResource(test.bootstrap.IFC4):
    def test_editing_a_resource(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcCrewResource")
        ifcopenshell.api.resource.edit_resource(
            self.file, resource=resource, attributes={"Name": "Zone A Crew"}
        )
        assert resource.Name == "Zone A Crew"


class TestEditResourceIFC2X3(test.bootstrap.IFC2X3, TestEditResource):
    pass
