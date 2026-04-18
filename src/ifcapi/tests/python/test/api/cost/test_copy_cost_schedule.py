
import ifcopenshell.api.cost
import ifcopenshell.util.cost
import test.bootstrap


class TestCopyCostSchedule(test.bootstrap.IFC4):
    def test_run(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo")
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        ifcopenshell.api.cost.add_cost_item(self.file, cost_item=item)
        old_cost_items = set(self.file.by_type("IfcCostItem"))

        new_schedule = ifcopenshell.api.cost.copy_cost_schedule(self.file, cost_schedule=schedule)

        assert new_schedule is not None
        assert new_schedule != schedule
        assert len(ifcopenshell.util.cost.get_root_cost_items(new_schedule)) == 1
        new_cost_items = set(ifcopenshell.util.cost.get_schedule_cost_items(new_schedule))
        assert len(new_cost_items) == 2
        assert len(new_cost_items.intersection(old_cost_items)) == 0


class TestCopyCostScheduleIFC2X3(test.bootstrap.IFC2X3, TestCopyCostSchedule):
    pass
