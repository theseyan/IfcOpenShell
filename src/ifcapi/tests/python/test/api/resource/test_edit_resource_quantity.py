# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import test.bootstrap


class TestEditResourceQuantity(test.bootstrap.IFC4):
    def test_editing_a_resource_quantity(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        quantity = ifcopenshell.api.resource.add_resource_quantity(
            self.file, resource=resource, ifc_class="IfcQuantityTime"
        )
        ifcopenshell.api.resource.edit_resource_quantity(
            self.file, physical_quantity=quantity, attributes={"TimeValue": 8.0}
        )
        assert quantity.TimeValue == 8.0
