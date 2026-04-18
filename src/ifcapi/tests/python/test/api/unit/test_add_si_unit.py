import ifcopenshell.api.unit
import test.bootstrap


class TestAddSIUnit(test.bootstrap.IFC4):
    def test_run(self):
        unit = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="MILLI")
        assert unit.UnitType == "LENGTHUNIT"
        assert unit.Name == "METRE"
        assert unit.Prefix == "MILLI"


class TestAddSIUnitIFC2X3(test.bootstrap.IFC2X3, TestAddSIUnit):
    pass
