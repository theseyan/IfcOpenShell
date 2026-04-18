
import ifcopenshell.api.cost
import ifcopenshell.util.element
import test.bootstrap


class TestAddCostItem(test.bootstrap.IFC4):
    def test_add_a_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        assert item1.is_a("IfcCostItem")
        assert item1.HasAssignments[0].is_a("IfcRelAssignsToControl")
        assert item1.HasAssignments[0].RelatingControl == schedule

    def test_add_a_sub_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        item2 = ifcopenshell.api.cost.add_cost_item(self.file, cost_item=item1)
        assert item2.is_a("IfcCostItem")
        assert ifcopenshell.util.element.get_nest(item2) == item1


class TestAddCostItemIFC2X3(test.bootstrap.IFC2X3, TestAddCostItem):
    pass
