import ifcopenshell.api.unit
import test.bootstrap


class TestUnassignUnit(test.bootstrap.IFC4):
    def test_run(self):
        project = self.file.createIfcProject()
        unit1 = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="USD")
        unit2 = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="JPY")
        assignment = ifcopenshell.api.unit.assign_unit(self.file, units=[unit1, unit2])
        ifcopenshell.api.unit.unassign_unit(self.file, units=[unit1])
        assert unit1 not in assignment.Units
        assert unit2 in assignment.Units

    def test_unassigning_the_last_unit(self):
        project = self.file.createIfcProject()
        unit = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="USD")
        ifcopenshell.api.unit.assign_unit(self.file, units=[unit])
        ifcopenshell.api.unit.unassign_unit(self.file, units=[unit])
        assert project.UnitsInContext is None

    def test_doing_nothing_if_the_unit_is_not_assigned(self):
        unit = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="USD")
        assert ifcopenshell.api.unit.unassign_unit(self.file, units=[unit]) is None


class TestUnassignUnitIFC2X3(test.bootstrap.IFC2X3, TestUnassignUnit):
    pass
