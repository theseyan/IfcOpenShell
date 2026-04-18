# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.cost
import test.bootstrap


class TestRemoveCostSchedule(test.bootstrap.IFC4):
    def test_remove_a_cost_schedule(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        ifcopenshell.api.cost.remove_cost_schedule(self.file, cost_schedule=schedule)
        assert not self.file.by_type("IfcCostSchedule")

    def test_remove_a_schedule_with_items(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        item2 = ifcopenshell.api.cost.add_cost_item(self.file, cost_item=item1)
        ifcopenshell.api.cost.remove_cost_schedule(self.file, cost_schedule=schedule)
        assert not self.file.by_type("IfcCostSchedule")
        assert not self.file.by_type("IfcCostItem")
        assert not self.file.by_type("IfcRelNests")
        assert not self.file.by_type("IfcRelAssignsToControl")


class TestRemoveCostScheduleIFC2X3(test.bootstrap.IFC2X3, TestRemoveCostSchedule):
    pass
