# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import ifcopenshell.api.unit
import pytest
import test.bootstrap


class TestAssignUnit(test.bootstrap.IFC4):
    def test_assign_default_metric_units(self):
        project = self.file.createIfcProject()
        assignment = ifcopenshell.api.unit.assign_unit(self.file)
        units = {unit.UnitType: unit for unit in assignment.Units}
        assert project.UnitsInContext == assignment
        assert set(units) == {"LENGTHUNIT", "AREAUNIT", "VOLUMEUNIT"}
        assert (units["LENGTHUNIT"].Prefix, units["LENGTHUNIT"].Name) == ("MILLI", "METRE")
        assert (units["AREAUNIT"].Prefix, units["AREAUNIT"].Name) == (None, "SQUARE_METRE")
        assert (units["VOLUMEUNIT"].Prefix, units["VOLUMEUNIT"].Name) == (None, "CUBIC_METRE")

    def test_an_explicit_empty_unit_list_uses_metric_defaults(self):
        self.file.createIfcProject()
        assignment = ifcopenshell.api.unit.assign_unit(self.file, units=[])
        assert {unit.UnitType for unit in assignment.Units} == {"LENGTHUNIT", "AREAUNIT", "VOLUMEUNIT"}

    def test_run(self):
        project = self.file.createIfcProject()
        unit1 = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="USD")
        unit2 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="MILLI")
        assignment = ifcopenshell.api.unit.assign_unit(self.file, units=[unit1, unit2])
        assert project.UnitsInContext == assignment
        assert assignment.is_a("IfcUnitAssignment")
        assert unit1 in assignment.Units
        assert unit2 in assignment.Units

    def test_assign_units_to_an_existing_assignment(self):
        project = self.file.createIfcProject()
        unit1 = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="USD")
        unit2 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="MILLI")
        assignment1 = ifcopenshell.api.unit.assign_unit(self.file, units=[unit1])
        assignment2 = ifcopenshell.api.unit.assign_unit(self.file, units=[unit2])
        assert project.UnitsInContext == assignment1
        assert assignment1 == assignment2
        assert unit1 in assignment1.Units
        assert unit2 in assignment1.Units

    def test_overwriting_an_existing_unit_type(self):
        project = self.file.createIfcProject()
        unit1 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="MILLI")
        unit2 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="CENTI")
        assignment1 = ifcopenshell.api.unit.assign_unit(self.file, units=[unit1])
        assignment2 = ifcopenshell.api.unit.assign_unit(self.file, units=[unit2])
        assert project.UnitsInContext == assignment1
        assert assignment1 == assignment2
        assert unit1 not in assignment1.Units
        assert unit2 in assignment1.Units

    def test_overwriting_an_existing_monetary_unit(self):
        project = self.file.createIfcProject()
        unit1 = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="USD")
        unit2 = ifcopenshell.api.unit.add_monetary_unit(self.file, currency="AUD")
        assignment1 = ifcopenshell.api.unit.assign_unit(self.file, units=[unit1])
        assignment2 = ifcopenshell.api.unit.assign_unit(self.file, units=[unit2])
        assert project.UnitsInContext == assignment1
        assert assignment1 == assignment2
        assert unit1 not in assignment1.Units
        assert unit2 in assignment1.Units

    def test_preserving_unrelated_assigned_units(self):
        self.file.createIfcProject()
        area = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="AREAUNIT")
        length1 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="MILLI")
        length2 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="CENTI")
        assignment = ifcopenshell.api.unit.assign_unit(self.file, units=[area, length1])
        ifcopenshell.api.unit.assign_unit(self.file, units=[length2])
        assert area in assignment.Units
        assert length1 not in assignment.Units
        assert length2 in assignment.Units

    def test_multiple_explicit_units_with_the_same_type_remain_members(self):
        self.file.createIfcProject()
        length1 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="MILLI")
        length2 = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT", prefix="CENTI")
        assignment = ifcopenshell.api.unit.assign_unit(self.file, units=[length1, length2, length1])
        assert set(assignment.Units) == {length1, length2}

    def test_custom_metric_convenience_units(self):
        self.file.createIfcProject()
        assignment = ifcopenshell.api.unit.assign_unit(
            self.file,
            length={"is_metric": True, "raw": "CENTIMETERS"},
            area={"is_metric": True, "raw": "MILLIMETERS"},
            volume={"is_metric": True, "raw": "KILOMETERS"},
        )
        units = {unit.UnitType: unit for unit in assignment.Units}
        assert (units["LENGTHUNIT"].Prefix, units["LENGTHUNIT"].Name) == ("CENTI", "METRE")
        assert (units["AREAUNIT"].Prefix, units["AREAUNIT"].Name) == ("MILLI", "SQUARE_METRE")
        assert (units["VOLUMEUNIT"].Prefix, units["VOLUMEUNIT"].Name) == ("KILO", "CUBIC_METRE")

    @pytest.mark.parametrize(
        ("raw", "base", "length_factor", "area_factor", "volume_factor"),
        [
            ("INCHES", "inch", 0.0254, 0.0006452, 0.00001639),
            ("FEET", "foot", 0.3048, 0.09290304, 0.02831684671168849),
            ("MILES", "mile", 1609, 2588881, 4165509529),
            ("THOU", "thou", 0.0000254, 6.4516e-10, 1.6387064e-14),
        ],
    )
    def test_imperial_convenience_units(self, raw, base, length_factor, area_factor, volume_factor):
        self.file.createIfcProject()
        assignment = ifcopenshell.api.unit.assign_unit(
            self.file,
            length={"is_metric": False, "raw": raw},
            area={"is_metric": False, "raw": raw},
            volume={"is_metric": False, "raw": raw},
        )
        units = {unit.UnitType: unit for unit in assignment.Units}
        expected = {
            "LENGTHUNIT": (base, 1, "METRE", length_factor),
            "AREAUNIT": (f"square {base}", 2, "SQUARE_METRE", area_factor),
            "VOLUMEUNIT": (f"cubic {base}", 3, "CUBIC_METRE", volume_factor),
        }
        for unit_type, (name, exponent, si_name, factor) in expected.items():
            unit = units[unit_type]
            assert unit.Name == name
            assert tuple(unit.Dimensions) == (exponent, 0, 0, 0, 0, 0, 0)
            assert unit.ConversionFactor.ValueComponent.wrappedValue == factor
            assert unit.ConversionFactor.UnitComponent.Name == si_name

    def test_failing_when_no_project_exists(self):
        with pytest.raises(RuntimeError):
            ifcopenshell.api.unit.assign_unit(self.file)


class TestAssignUnitIFC2X3(test.bootstrap.IFC2X3, TestAssignUnit):
    pass


class TestAssignUnitIFC4X3(test.bootstrap.IFC4X3, TestAssignUnit):
    pass
