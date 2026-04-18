# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/resource/test_add_resource_quantity.py

import pytest

import ifcopenshell.api.resource
from ifcopenshell.api.resource.add_resource_quantity import RESOURCES_TO_QUANTITIES
import test.bootstrap


class TestAddResourceQuantity(test.bootstrap.IFC4):
    def test_adding_a_supported_quantity(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        quantity = ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityTime"
        )
        assert quantity.is_a("IfcQuantityTime")
        assert quantity.Name == "Unnamed"
        assert quantity[3] == 0.0
        assert resource.BaseQuantity == quantity

    def test_unsupported_quantity_raises(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        with pytest.raises(ValueError):
            ifcopenshell.api.resource.add_resource_quantity(
                self.file, resource=resource, ifc_class="IfcQuantityArea"
            )

    def test_previous_quantity_is_replaced(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcConstructionMaterialResource")
        q1 = ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityArea"
        )
        q2 = ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityLength"
        )
        assert resource.BaseQuantity == q2
        assert len(self.file.by_type("IfcPhysicalSimpleQuantity")) == 1

    def test_all_resource_types_with_supported_quantities(self):
        self.file.create_entity("IfcProject")
        for resource_type, quantity_types in RESOURCES_TO_QUANTITIES.items():
            resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class=resource_type)
            for quantity_type in quantity_types:
                quantity = ifcopenshell.api.resource.add_resource_quantity(
                    self.file, resource=resource, ifc_class=quantity_type
                )
                assert quantity.is_a(quantity_type)
                assert quantity.Name == "Unnamed"
                assert resource.BaseQuantity == quantity
            ifcopenshell.api.resource.remove_resource(self.file, resource=resource)

    def test_count_quantity_has_integer_zero(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcConstructionProductResource")
        quantity = ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityCount"
        )
        assert quantity[3] == 0


class TestAddResourceQuantityIFC2X3(test.bootstrap.IFC2X3, TestAddResourceQuantity):
    pass
