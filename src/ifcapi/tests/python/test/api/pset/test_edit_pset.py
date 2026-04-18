# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for pset.edit_pset — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestEditPset(test.bootstrap.IFC4):
    def test_editing_pset_name(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, name="Bar")
        assert pset.Name == "Bar"

    def test_adding_string_property(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"FireRating": "2HR"})
        props = ifcopenshell.util.element.get_psets(element)
        assert props["Foo"]["FireRating"] == "2HR"

    def test_adding_float_property(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"ThermalValue": 42.5})
        props = ifcopenshell.util.element.get_psets(element)
        assert abs(props["Foo"]["ThermalValue"] - 42.5) < 0.001

    def test_adding_bool_property(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Combustible": False})
        props = ifcopenshell.util.element.get_psets(element)
        assert props["Foo"]["Combustible"] is False

    def test_adding_int_property(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Count": 5})
        props = ifcopenshell.util.element.get_psets(element)
        assert props["Foo"]["Count"] == 5

    def test_editing_existing_property(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Rating": "A"})
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Rating": "B"})
        props = ifcopenshell.util.element.get_psets(element)
        assert props["Foo"]["Rating"] == "B"

    def test_editing_preserves_type(self):
        """When editing an existing IfcLabel property to a new string, it stays IfcLabel."""
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Desc": "hello"})
        # NominalValue should be IfcLabel
        prop = pset.HasProperties[0]
        assert prop.NominalValue.is_a() == "IfcLabel"
        # Edit to a new value — type preserved
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Desc": "world"})
        prop = next(p for p in pset.HasProperties if p.Name == "Desc")
        assert prop.NominalValue.is_a() == "IfcLabel"
        assert prop.NominalValue.wrappedValue == "world"

    def test_purge_property(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"A": "1", "B": "2"})
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"B": None}, should_purge=True)
        props = ifcopenshell.util.element.get_psets(element)
        assert "A" in props["Foo"]
        assert "B" not in props["Foo"]

    def test_adding_multiple_properties(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={
            "A": "one",
            "B": 2.0,
            "C": True,
            "D": 42,
        })
        props = ifcopenshell.util.element.get_psets(element)["Foo"]
        assert props["A"] == "one"
        assert abs(props["B"] - 2.0) < 0.001
        assert props["C"] is True
        assert props["D"] == 42

    def test_explicit_typed_value(self):
        """Setting NominalValue to an explicit typed entity."""
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        label = self.file.create_entity("IfcLabel", "Custom")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Explicit": label})
        props = ifcopenshell.util.element.get_psets(element)
        assert props["Foo"]["Explicit"] == "Custom"

    def test_edit_pset_on_type(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"X": "Y"})
        props = ifcopenshell.util.element.get_psets(element)
        assert props["Foo"]["X"] == "Y"


class TestEditPsetIFC2X3(test.bootstrap.IFC2X3, TestEditPset):
    pass
