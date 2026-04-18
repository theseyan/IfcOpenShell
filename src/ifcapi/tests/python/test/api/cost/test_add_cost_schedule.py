
import ifcopenshell.api.cost
import test.bootstrap


class TestAddCostSchedule(test.bootstrap.IFC4):
    def test_add_a_cost_schedule(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="BUDGET")
        assert schedule.is_a("IfcCostSchedule")
        assert schedule.Name == "Foo"
        assert schedule.PredefinedType == "BUDGET"

    def test_adding_a_userdefined_type(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file, name="Foo", predefined_type="FOO")
        assert schedule.is_a("IfcCostSchedule")
        assert schedule.Name == "Foo"
        assert schedule.PredefinedType == "USERDEFINED"
        assert schedule.ObjectType == "FOO"


class TestAddCostScheduleIFC2X3(test.bootstrap.IFC2X3, TestAddCostSchedule):
    pass
