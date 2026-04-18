# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.cost
import test.bootstrap


class TestRemoveCostValue(test.bootstrap.IFC4):
    def test_remove_cost_value(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        ifcopenshell.api.cost.remove_cost_value(self.file, parent=item, cost_value=value)
        assert not item.CostValues

    def test_remove_cost_value_subcomponent(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        sub = ifcopenshell.api.cost.add_cost_value(self.file, parent=value)
        ifcopenshell.api.cost.remove_cost_value(self.file, parent=value, cost_value=sub)
        assert not value.Components
