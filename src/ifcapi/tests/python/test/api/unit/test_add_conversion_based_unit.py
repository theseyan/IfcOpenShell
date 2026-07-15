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


CONVERSION_UNITS = [
    ("thou", "LENGTHUNIT", (1, 0, 0, 0, 0, 0, 0), 0.0000254, "METRE", None),
    ("inch", "LENGTHUNIT", (1, 0, 0, 0, 0, 0, 0), 0.0254, "METRE", None),
    ("foot", "LENGTHUNIT", (1, 0, 0, 0, 0, 0, 0), 0.3048, "METRE", None),
    ("yard", "LENGTHUNIT", (1, 0, 0, 0, 0, 0, 0), 0.914, "METRE", None),
    ("mile", "LENGTHUNIT", (1, 0, 0, 0, 0, 0, 0), 1609, "METRE", None),
    ("square thou", "AREAUNIT", (2, 0, 0, 0, 0, 0, 0), 6.4516e-10, "SQUARE_METRE", None),
    ("square inch", "AREAUNIT", (2, 0, 0, 0, 0, 0, 0), 0.0006452, "SQUARE_METRE", None),
    ("square foot", "AREAUNIT", (2, 0, 0, 0, 0, 0, 0), 0.09290304, "SQUARE_METRE", None),
    ("square yard", "AREAUNIT", (2, 0, 0, 0, 0, 0, 0), 0.83612736, "SQUARE_METRE", None),
    ("acre", "AREAUNIT", (2, 0, 0, 0, 0, 0, 0), 4046.86, "SQUARE_METRE", None),
    ("square mile", "AREAUNIT", (2, 0, 0, 0, 0, 0, 0), 2588881, "SQUARE_METRE", None),
    ("cubic thou", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 1.6387064e-14, "CUBIC_METRE", None),
    ("cubic inch", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.00001639, "CUBIC_METRE", None),
    ("cubic foot", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.02831684671168849, "CUBIC_METRE", None),
    ("cubic yard", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.7636, "CUBIC_METRE", None),
    ("cubic mile", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 4165509529, "CUBIC_METRE", None),
    ("litre", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.001, "CUBIC_METRE", None),
    ("fluid ounce UK", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.0000284130625, "CUBIC_METRE", None),
    ("fluid ounce US", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.00002957353, "CUBIC_METRE", None),
    ("pint UK", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.000568, "CUBIC_METRE", None),
    ("pint US", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.000473, "CUBIC_METRE", None),
    ("gallon UK", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.004546, "CUBIC_METRE", None),
    ("gallon US", "VOLUMEUNIT", (3, 0, 0, 0, 0, 0, 0), 0.003785, "CUBIC_METRE", None),
    ("degree", "PLANEANGLEUNIT", (0, 0, 0, 0, 0, 0, 0), 0.017453292519943295, "RADIAN", None),
    ("ounce", "MASSUNIT", (0, 1, 0, 0, 0, 0, 0), 0.02835, "GRAM", "KILO"),
    ("pound", "MASSUNIT", (0, 1, 0, 0, 0, 0, 0), 0.454, "GRAM", "KILO"),
    ("ton UK", "MASSUNIT", (0, 1, 0, 0, 0, 0, 0), 1016.0469088, "GRAM", "KILO"),
    ("ton US", "MASSUNIT", (0, 1, 0, 0, 0, 0, 0), 907.18474, "GRAM", "KILO"),
    ("tonne", "MASSUNIT", (0, 1, 0, 0, 0, 0, 0), 1000.0, "GRAM", "KILO"),
    ("lbf", "FORCEUNIT", (1, 1, -2, 0, 0, 0, 0), 4.4482216153, "NEWTON", None),
    ("kip", "FORCEUNIT", (1, 1, -2, 0, 0, 0, 0), 4448.2216153, "NEWTON", None),
    ("psi", "PRESSUREUNIT", (-1, 1, -2, 0, 0, 0, 0), 6894.7572932, "PASCAL", None),
    ("ksi", "PRESSUREUNIT", (-1, 1, -2, 0, 0, 0, 0), 6894757.2932, "PASCAL", None),
    ("minute", "TIMEUNIT", (0, 0, 1, 0, 0, 0, 0), 60, "SECOND", None),
    ("hour", "TIMEUNIT", (0, 0, 1, 0, 0, 0, 0), 3600, "SECOND", None),
    ("day", "TIMEUNIT", (0, 0, 1, 0, 0, 0, 0), 86400, "SECOND", None),
    ("btu", "ENERGYUNIT", (2, 1, -2, 0, 0, 0, 0), 1055.056, "JOULE", None),
]


class TestAddConversionBasedUnitIFC2X3(test.bootstrap.IFC2X3):
    def test_default_name_is_foot(self):
        assert ifcopenshell.api.unit.add_conversion_based_unit(self.file).Name == "foot"

    @pytest.mark.parametrize(("name", "unit_type", "dimensions", "factor", "si_name", "prefix"), CONVERSION_UNITS)
    def test_complete_conversion_inventory(self, name, unit_type, dimensions, factor, si_name, prefix):
        unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name=name)
        assert unit.UnitType == unit_type
        assert tuple(unit.Dimensions) == dimensions
        assert unit.ConversionFactor.ValueComponent.wrappedValue == pytest.approx(factor)
        assert unit.ConversionFactor.UnitComponent.Name == si_name
        assert unit.ConversionFactor.UnitComponent.Prefix == prefix

    def test_run(self):
        unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name="foot")
        assert unit.is_a("IfcConversionBasedUnit")
        assert unit.Dimensions.LengthExponent == 1
        assert unit.Dimensions.MassExponent == 0
        assert unit.Dimensions.TimeExponent == 0
        assert unit.Dimensions.ElectricCurrentExponent == 0
        assert unit.Dimensions.ThermodynamicTemperatureExponent == 0
        assert unit.Dimensions.AmountOfSubstanceExponent == 0
        assert unit.Dimensions.LuminousIntensityExponent == 0
        assert unit.UnitType == "LENGTHUNIT"
        assert unit.Name == "foot"
        assert unit.ConversionFactor.ValueComponent.wrappedValue == 0.3048
        si_unit = unit.ConversionFactor.UnitComponent
        assert si_unit.is_a("IfcSIUnit")
        assert si_unit.UnitType == "LENGTHUNIT"
        assert si_unit.Prefix is None
        assert si_unit.Name == "METRE"

    def test_adding_mass_units_creates_proper_massunit(self):
        mass_units = [
            ("tonne", 1000.0),
            ("pound", 0.454),
            ("ounce", 0.02835),
            ("ton UK", 1016.0469088),
            ("ton US", 907.18474),
        ]

        for name, expected_conversion in mass_units:
            unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name=name)

            assert unit.is_a("IfcConversionBasedUnit")
            assert unit.UnitType == "MASSUNIT"
            assert unit.Name == name

            actual_conversion = unit.ConversionFactor.ValueComponent.wrappedValue
            assert actual_conversion == expected_conversion

            target_unit = unit.ConversionFactor.UnitComponent
            assert target_unit.is_a("IfcSIUnit")
            assert target_unit.UnitType == "MASSUNIT"
            assert target_unit.Name == "GRAM"
            assert target_unit.Prefix == "KILO"

    def test_adding_time_units_creates_proper_timeunit(self):
        time_units = [
            ("minute", 60),
            ("hour", 3600),
            ("day", 86400),
        ]

        for name, expected_conversion in time_units:
            unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name=name)

            assert unit.is_a("IfcConversionBasedUnit")
            assert unit.UnitType == "TIMEUNIT"
            assert unit.Name == name

            actual_conversion = unit.ConversionFactor.ValueComponent.wrappedValue
            assert actual_conversion == expected_conversion

            target_unit = unit.ConversionFactor.UnitComponent
            assert target_unit.is_a("IfcSIUnit")
            assert target_unit.UnitType == "TIMEUNIT"
            assert target_unit.Name == "SECOND"
            assert target_unit.Prefix is None


class TestAddConversionBasedUnitIFC4(test.bootstrap.IFC4, TestAddConversionBasedUnitIFC2X3):
    def test_adding_a_unit_with_offset(self):
        unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name="fahrenheit")
        assert unit.is_a("IfcConversionBasedUnitWithOffset")
        assert unit.Dimensions.LengthExponent == 0
        assert unit.Dimensions.MassExponent == 0
        assert unit.Dimensions.TimeExponent == 0
        assert unit.Dimensions.ElectricCurrentExponent == 0
        assert unit.Dimensions.ThermodynamicTemperatureExponent == 1
        assert unit.Dimensions.AmountOfSubstanceExponent == 0
        assert unit.Dimensions.LuminousIntensityExponent == 0
        assert unit.UnitType == "THERMODYNAMICTEMPERATUREUNIT"
        assert unit.Name == "fahrenheit"
        assert unit.ConversionFactor.ValueComponent.wrappedValue == 1.8
        si_unit = unit.ConversionFactor.UnitComponent
        assert si_unit.is_a("IfcSIUnit")
        assert si_unit.UnitType == "THERMODYNAMICTEMPERATUREUNIT"
        assert si_unit.Prefix is None
        assert si_unit.Name == "KELVIN"
        assert unit.ConversionOffset == -459.67

    def test_unknown_units_fall_back_to_userdefined(self):
        unknown_unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name="unknown_unit")
        assert unknown_unit.UnitType == "USERDEFINED"
        assert unknown_unit.Name == "unknown_unit"
        assert tuple(unknown_unit.Dimensions) == (0, 0, 0, 0, 0, 0, 0)
        assert unknown_unit.ConversionFactor.ValueComponent.wrappedValue == 1
        assert unknown_unit.ConversionFactor.UnitComponent.Name == "METRE"

    def test_explicit_nonzero_offset_overrides_builtin_offset(self):
        unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name="fahrenheit", conversion_offset=12.5)
        assert unit.is_a("IfcConversionBasedUnitWithOffset")
        assert unit.ConversionOffset == 12.5

    def test_explicit_zero_offset_uses_builtin_offset(self):
        unit = ifcopenshell.api.unit.add_conversion_based_unit(self.file, name="fahrenheit", conversion_offset=0)
        assert unit.is_a("IfcConversionBasedUnitWithOffset")
        assert unit.ConversionOffset == -459.67


class TestAddConversionBasedUnitIFC4X3(test.bootstrap.IFC4X3, TestAddConversionBasedUnitIFC4):
    pass


class TestAddConversionBasedUnitOffsetIFC2X3(test.bootstrap.IFC2X3):
    def test_offset_subtype_is_unavailable(self):
        with pytest.raises(RuntimeError):
            ifcopenshell.api.unit.add_conversion_based_unit(self.file, name="fahrenheit")
