# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

import ifcopenshell.api.unit

import test.bootstrap


class TestAddDerivedUnit(test.bootstrap.IFC4):
    def test_pairs_each_component_unit_with_its_exponent(self):
        length = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="LENGTHUNIT")
        time = ifcopenshell.api.unit.add_si_unit(self.file, unit_type="TIMEUNIT")

        unit = ifcopenshell.api.unit.add_derived_unit(
            self.file,
            unit_type="LINEARVELOCITYUNIT",
            userdefinedtype=None,
            attributes={length: 1, time: -1},
        )

        assert unit.is_a("IfcDerivedUnit")
        assert [(element.Unit, element.Exponent) for element in unit.Elements] == [
            (length, 1),
            (time, -1),
        ]
        assert unit.UnitType == "LINEARVELOCITYUNIT"
        assert unit.UserDefinedType is None


class TestAddDerivedUnitIFC2X3(test.bootstrap.IFC2X3, TestAddDerivedUnit):
    pass
