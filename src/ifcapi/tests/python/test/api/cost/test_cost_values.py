# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.cost
import test.bootstrap


class TestAssignCostValue(test.bootstrap.IFC4):
    def test_assign_cost_value_from_rate(self):
        rate_tables = ifcopenshell.api.cost.add_cost_schedule(
            self.file, predefined_type="SCHEDULEOFRATES"
        )
        rate = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=rate_tables)
        rate_value = ifcopenshell.api.cost.add_cost_value(self.file, parent=rate)
        ifcopenshell.api.cost.edit_cost_value(
            self.file, cost_value=rate_value, attributes={"AppliedValue": 5.0}
        )

        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        ifcopenshell.api.cost.assign_cost_value(self.file, cost_item=item, cost_rate=rate)
        assert item.CostValues == rate.CostValues


class TestCopyCostItemValues(test.bootstrap.IFC4):
    def test_copy_cost_item_values(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        item2 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item1)
        ifcopenshell.api.cost.edit_cost_value(
            self.file, cost_value=value, attributes={"AppliedValue": 5000.0}
        )
        ifcopenshell.api.cost.copy_cost_item_values(self.file, source=item1, destination=item2)
        assert item2.CostValues
        assert len(item2.CostValues) == 1
        # The copied values should be different entities
        assert item2.CostValues[0] != item1.CostValues[0]


class TestEditCostValueFormula(test.bootstrap.IFC4):
    def test_edit_cost_value_formula(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        ifcopenshell.api.cost.edit_cost_value_formula(
            self.file, cost_value=value, formula="5000*1.19"
        )
        assert value.ArithmeticOperator == "MULTIPLY"
        assert value.Components
        assert len(value.Components) == 2
