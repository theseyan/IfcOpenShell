# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import test.bootstrap


class TestAddResourceTime(test.bootstrap.IFC4):
    def test_adding_resource_time(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        time = ifcopenshell.api.resource.add_resource_time(self.file, resource=resource)
        assert time.is_a("IfcResourceTime")
        assert resource.Usage == time
