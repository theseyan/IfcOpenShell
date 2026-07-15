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
import test.bootstrap


class TestEditNamedUnitIFC2X3(test.bootstrap.IFC2X3):
    def test_copying_shared_dimensions_before_editing(self):
        dimensions = self.file.createIfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0)
        unit = self.file.createIfcContextDependentUnit(dimensions, "LENGTHUNIT", "A")
        other = self.file.createIfcContextDependentUnit(dimensions, "LENGTHUNIT", "B")
        ifcopenshell.api.unit.edit_named_unit(self.file, unit=unit, attributes={"Dimensions": (2, 3, 4, 5, 6, 7, 8)})
        assert unit.Dimensions != dimensions
        assert tuple(unit.Dimensions) == (2, 3, 4, 5, 6, 7, 8)
        assert other.Dimensions == dimensions
        assert tuple(other.Dimensions) == (1, 0, 0, 0, 0, 0, 0)

    def test_mutating_uniquely_owned_dimensions_in_place(self):
        dimensions = self.file.createIfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0)
        unit = self.file.createIfcContextDependentUnit(dimensions, "LENGTHUNIT", "A")
        dimensions_id = dimensions.id()
        ifcopenshell.api.unit.edit_named_unit(self.file, unit=unit, attributes={"Dimensions": (2, 3, 4, 5, 6, 7, 8)})
        assert unit.Dimensions.id() == dimensions_id
        assert tuple(unit.Dimensions) == (2, 3, 4, 5, 6, 7, 8)

    def test_short_dimensions_mutate_a_single_owner_partially(self):
        unit = self.file.createIfcContextDependentUnit(
            self.file.createIfcDimensionalExponents(), "USERDEFINED", "A"
        )
        ifcopenshell.api.unit.edit_named_unit(self.file, unit=unit, attributes={"Dimensions": (1, 2)})
        assert tuple(unit.Dimensions) == (1, 2, None, None, None, None, None)

    def test_short_shared_dimensions_create_a_partial_copy(self):
        dimensions = self.file.createIfcDimensionalExponents()
        unit = self.file.createIfcContextDependentUnit(dimensions, "USERDEFINED", "A")
        other = self.file.createIfcContextDependentUnit(dimensions, "USERDEFINED", "B")
        ifcopenshell.api.unit.edit_named_unit(self.file, unit=unit, attributes={"Dimensions": (1, 2)})
        assert unit.Dimensions != dimensions
        assert tuple(unit.Dimensions) == (1, 2, None, None, None, None, None)
        assert other.Dimensions == dimensions
        assert tuple(dimensions) == (None, None, None, None, None, None, None)

    def test_edit_context_dependent_unit(self):
        unit = self.file.createIfcContextDependentUnit()
        unit.Dimensions = self.file.createIfcDimensionalExponents()
        ifcopenshell.api.unit.edit_named_unit(
            self.file,
            unit=unit,
            attributes={"Dimensions": (1, 2, 3, 4, 5, 6, 7), "UnitType": "LENGTHUNIT", "Name": "Name"},
        )
        assert [a for a in unit.Dimensions] == [1, 2, 3, 4, 5, 6, 7]
        assert unit.UnitType == "LENGTHUNIT"
        assert unit.Name == "Name"

    def test_edit_si_unit(self):
        unit = self.file.createIfcSIUnit()
        ifcopenshell.api.unit.edit_named_unit(
            self.file,
            unit=unit,
            attributes={"UnitType": "LENGTHUNIT", "Prefix": "MILLI", "Name": "METRE"},
        )
        assert unit.UnitType == "LENGTHUNIT"
        assert unit.Prefix == "MILLI"
        assert unit.Name == "METRE"

    def test_edit_conversion_based_unit(self):
        unit = self.file.createIfcConversionBasedUnit()
        unit.Dimensions = self.file.createIfcDimensionalExponents()
        ifcopenshell.api.unit.edit_named_unit(
            self.file,
            unit=unit,
            attributes={"Dimensions": (1, 2, 3, 4, 5, 6, 7), "UnitType": "LENGTHUNIT", "Name": "Name"},
        )
        assert [a for a in unit.Dimensions] == [1, 2, 3, 4, 5, 6, 7]
        assert unit.UnitType == "LENGTHUNIT"
        assert unit.Name == "Name"


class TestEditNamedUnitIFC4(test.bootstrap.IFC4, TestEditNamedUnitIFC2X3):
    def test_edit_conversion_based_unit_with_offset(self):
        unit = self.file.createIfcConversionBasedUnitWithOffset()
        unit.Dimensions = self.file.createIfcDimensionalExponents()
        ifcopenshell.api.unit.edit_named_unit(
            self.file,
            unit=unit,
            attributes={
                "Dimensions": (1, 2, 3, 4, 5, 6, 7),
                "UnitType": "LENGTHUNIT",
                "Name": "Name",
                "ConversionOffset": 1,
            },
        )
        assert [a for a in unit.Dimensions] == [1, 2, 3, 4, 5, 6, 7]
        assert unit.UnitType == "LENGTHUNIT"
        assert unit.Name == "Name"
        assert unit.ConversionOffset == 1
