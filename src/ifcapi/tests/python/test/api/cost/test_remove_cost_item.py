
import ifcopenshell.api.cost
import test.bootstrap


class TestRemoveCostItem(test.bootstrap.IFC4):
    def test_remove_a_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        ifcopenshell.api.cost.remove_cost_item(self.file, cost_item=item1)
        assert not self.file.by_type("IfcCostItem")
        assert not self.file.by_type("IfcRelAssignsToControl")

    def test_remove_a_sub_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        item2 = ifcopenshell.api.cost.add_cost_item(self.file, cost_item=item1)
        ifcopenshell.api.cost.remove_cost_item(self.file, cost_item=item2)
        assert self.file.by_type("IfcCostItem") == [item1]
        assert self.file.by_type("IfcRelAssignsToControl")
        assert not self.file.by_type("IfcRelNests")

    def test_remove_a_parent_cost_item(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        item2 = ifcopenshell.api.cost.add_cost_item(self.file, cost_item=item1)
        ifcopenshell.api.cost.remove_cost_item(self.file, cost_item=item1)
        assert not self.file.by_type("IfcCostItem")
        assert not self.file.by_type("IfcRelAssignsToControl")
        assert not self.file.by_type("IfcRelNests")

    def test_remove_cost_item_keep_rel_for_other_cost_items(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        item1 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        item2 = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        ifcopenshell.api.cost.remove_cost_item(self.file, cost_item=item1)
        assert self.file.by_type("IfcCostItem") == [item2]
        rel = next(iter(self.file.by_type("IfcRelAssignsToControl")), None)
        assert rel
        assert rel.RelatedObjects == (item2,)


class TestRemoveCostItemIFC2X3(test.bootstrap.IFC2X3, TestRemoveCostItem):
    pass
