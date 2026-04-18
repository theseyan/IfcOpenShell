# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/resource/test_remove_resource_quantity.py

import ifcopenshell.api.resource
import test.bootstrap


class TestRemoveResourceQuantity(test.bootstrap.IFC4):
    def test_removing_a_resource_quantity(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityTime"
        )
        assert resource.BaseQuantity is not None
        ifcopenshell.api.resource.remove_resource_quantity(self.file, resource=resource)
        assert resource.BaseQuantity is None
        assert len(self.file.by_type("IfcPhysicalSimpleQuantity")) == 0

    def test_removing_a_resource_quantity_when_none_exists(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        # Should not raise.
        ifcopenshell.api.resource.remove_resource_quantity(self.file, resource=resource)
        assert resource.BaseQuantity is None


class TestRemoveResourceQuantityIFC2X3(test.bootstrap.IFC2X3, TestRemoveResourceQuantity):
    pass
