# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.cost
import test.bootstrap


class TestAddCostValue(test.bootstrap.IFC4):
    def test_add_cost_value_to_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        assert value.is_a("IfcCostValue")
        assert item.CostValues == (value,)

    def test_add_cost_value_subcomponent(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        subvalue = ifcopenshell.api.cost.add_cost_value(self.file, parent=value)
        assert subvalue.is_a("IfcCostValue")
        assert value.Components == (subvalue,)
