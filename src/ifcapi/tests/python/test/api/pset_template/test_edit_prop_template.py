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

import ifcopenshell.api.pset_template
import pytest

import test.bootstrap


class TestEditPropTemplate(test.bootstrap.IFC4):
    def test_editing_a_simple_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(
            self.file, name="ABC_RiskFactors"
        )
        prop = ifcopenshell.api.pset_template.add_prop_template(
            self.file, pset_template=template
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file,
            prop_template=prop,
            attributes={"Name": "DemoA", "PrimaryMeasureType": "IfcLabel"},
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop, attributes={"Name": "DemoB"}
        )
        assert prop.Name == "DemoB"

    def test_editing_an_enumeration(self):
        template = ifcopenshell.api.pset_template.add_pset_template(
            self.file, name="ABC_RiskFactors"
        )
        prop = ifcopenshell.api.pset_template.add_prop_template(
            self.file, pset_template=template
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file,
            prop_template=prop,
            attributes={"Name": "DemoA", "PrimaryMeasureType": "IfcLabel"},
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file,
            prop_template=prop,
            attributes={"Enumerators": ["FOO", "BAR"]},
        )
        assert prop.Enumerators.EnumerationValues == tuple(
            self.file.createIfcLabel(v) for v in ("FOO", "BAR")
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file,
            prop_template=prop,
            attributes={"Name": "DemoC", "Enumerators": ["BAZ", "BAR"]},
        )
        assert prop.Enumerators.Name == "DemoC"
        assert prop.Enumerators.EnumerationValues == tuple(
            self.file.createIfcLabel(v) for v in ("BAZ", "BAR")
        )
        assert len(self.file.by_type("IfcPropertyEnumeration")) == 1

    @pytest.mark.parametrize(
        ("measure_type", "values"),
        [
            ("IfcLabel", ["A", "B"]),
            ("IfcIdentifier", ["A-1", "B-2"]),
            ("IfcInteger", [1, 2]),
            ("IfcReal", [1.25, 2.5]),
            ("IfcLengthMeasure", [1.25, 2.5]),
            ("IfcBoolean", [True, False]),
        ],
    )
    def test_preserving_typed_enumerator_values(self, measure_type, values):
        template = ifcopenshell.api.pset_template.add_pset_template(
            self.file, name="Typed"
        )
        prop = ifcopenshell.api.pset_template.add_prop_template(
            self.file, pset_template=template
        )
        attributes = {"PrimaryMeasureType": measure_type, "Enumerators": values}
        original = {
            key: value.copy() if isinstance(value, list) else value
            for key, value in attributes.items()
        }

        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop, attributes=attributes
        )

        assert attributes == original
        assert prop.PrimaryMeasureType == measure_type
        assert [value.is_a() for value in prop.Enumerators.EnumerationValues] == [
            measure_type
        ] * len(values)
        assert [
            value.wrappedValue for value in prop.Enumerators.EnumerationValues
        ] == values

    @pytest.mark.parametrize("enumerators", [None, []])
    def test_null_and_empty_enumerators_leave_existing_enumeration_unchanged(
        self, enumerators
    ):
        template = ifcopenshell.api.pset_template.add_pset_template(
            self.file, name="Typed"
        )
        prop = ifcopenshell.api.pset_template.add_prop_template(
            self.file, pset_template=template
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file,
            prop_template=prop,
            attributes={"Name": "Old", "Enumerators": ["A"]},
        )
        enumeration = prop.Enumerators

        attributes = {"Name": "New", "Enumerators": enumerators}
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop, attributes=attributes
        )

        assert prop.Name == "New"
        assert prop.Enumerators.id() == enumeration.id()
        assert prop.Enumerators.Name == "Old"
        assert [value.wrappedValue for value in prop.Enumerators.EnumerationValues] == [
            "A"
        ]
        assert attributes == {"Name": "New", "Enumerators": enumerators}

    def test_rejecting_invalid_values_without_corrupting_an_existing_enumeration(self):
        template = ifcopenshell.api.pset_template.add_pset_template(
            self.file, name="Typed"
        )
        prop = ifcopenshell.api.pset_template.add_prop_template(
            self.file, pset_template=template
        )
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file,
            prop_template=prop,
            attributes={"Name": "Old", "Enumerators": ["A"]},
        )
        enumeration = prop.Enumerators

        with pytest.raises(RuntimeError, match="Invalid PrimaryMeasureType"):
            ifcopenshell.api.pset_template.edit_prop_template(
                self.file,
                prop_template=prop,
                attributes={
                    "PrimaryMeasureType": "IfcDoesNotExist",
                    "Enumerators": ["B"],
                },
            )
        with pytest.raises(RuntimeError, match="incompatible"):
            ifcopenshell.api.pset_template.edit_prop_template(
                self.file,
                prop_template=prop,
                attributes={"PrimaryMeasureType": "IfcInteger", "Enumerators": ["B"]},
            )

        assert prop.Enumerators.id() == enumeration.id()
        assert prop.Enumerators.Name == "Old"
        assert [value.wrappedValue for value in prop.Enumerators.EnumerationValues] == [
            "A"
        ]
        assert prop.PrimaryMeasureType == "IfcLabel"


class TestEditPropTemplateIFC4X3(test.bootstrap.IFC4X3, TestEditPropTemplate):
    pass
