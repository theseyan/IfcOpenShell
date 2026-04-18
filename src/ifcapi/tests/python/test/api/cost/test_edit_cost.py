# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.cost
import test.bootstrap


class TestEditCostItem(test.bootstrap.IFC4):
    def test_edit_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        ifcopenshell.api.cost.edit_cost_item(self.file, cost_item=item, attributes={"Name": "Foo"})
        assert item.Name == "Foo"


class TestEditCostSchedule(test.bootstrap.IFC4):
    def test_edit_cost_schedule(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        ifcopenshell.api.cost.edit_cost_schedule(
            self.file, cost_schedule=schedule, attributes={"Name": "Foo"}
        )
        assert schedule.Name == "Foo"


class TestEditCostItemQuantity(test.bootstrap.IFC4):
    def test_edit_cost_item_quantity(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        quantity = ifcopenshell.api.cost.add_cost_item_quantity(
            self.file, cost_item=item, ifc_class="IfcQuantityVolume"
        )
        ifcopenshell.api.cost.edit_cost_item_quantity(
            self.file, physical_quantity=quantity, attributes={"VolumeValue": 3.0}
        )
        assert quantity[3] == 3.0
