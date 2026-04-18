# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.cost
import ifcopenshell.api.unit
import test.bootstrap


class TestEditCostValue(test.bootstrap.IFC4):
    def test_editing_applied_value(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        ifcopenshell.api.cost.edit_cost_value(self.file, cost_value=value, attributes={"AppliedValue": 42.0})
        assert value.AppliedValue.wrappedValue == 42.0

    def test_editing_unit_basis_removes_old_deeply(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        unit = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT")
        ifcopenshell.api.cost.edit_cost_value(
            self.file,
            cost_value=value,
            attributes={"UnitBasis": {"ValueComponent": 1.0, "UnitComponent": unit}},
        )
        old_basis = value.UnitBasis
        assert old_basis is not None
        old_basis_id = old_basis.id()
        ifcopenshell.api.cost.edit_cost_value(
            self.file,
            cost_value=value,
            attributes={"UnitBasis": {"ValueComponent": 2.0, "UnitComponent": unit}},
        )
        assert value.UnitBasis is not None
        assert value.UnitBasis.id() != old_basis_id

    def test_clearing_unit_basis(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        value = ifcopenshell.api.cost.add_cost_value(self.file, parent=item)
        unit = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT")
        ifcopenshell.api.cost.edit_cost_value(
            self.file,
            cost_value=value,
            attributes={"UnitBasis": {"ValueComponent": 1.0, "UnitComponent": unit}},
        )
        assert value.UnitBasis is not None
        ifcopenshell.api.cost.edit_cost_value(
            self.file, cost_value=value, attributes={"UnitBasis": None}
        )
        assert value.UnitBasis is None


class TestEditCostValueIFC4X3(test.bootstrap.IFC4X3, TestEditCostValue):
    pass
