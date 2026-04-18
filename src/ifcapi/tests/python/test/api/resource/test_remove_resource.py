# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/resource/test_remove_resource.py

import ifcopenshell.api.resource
import test.bootstrap


class TestRemoveResource(test.bootstrap.IFC4):
    def test_removing_a_simple_resource(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        ifcopenshell.api.resource.remove_resource(self.file, resource=resource)
        assert len(self.file.by_type("IfcLaborResource")) == 0

    def test_removing_a_resource_with_quantity(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityTime"
        )
        ifcopenshell.api.resource.remove_resource(self.file, resource=resource)
        assert len(self.file.by_type("IfcLaborResource")) == 0
        assert len(self.file.by_type("IfcPhysicalSimpleQuantity")) == 0


class TestRemoveResourceIFC2X3(test.bootstrap.IFC2X3, TestRemoveResource):
    pass
