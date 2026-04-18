# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for pset.add_pset — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestAddPset(test.bootstrap.IFC4):
    def test_adding_a_pset_to_an_object(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Pset_WallCommon")
        assert pset.is_a("IfcPropertySet")
        assert "Pset_WallCommon" in ifcopenshell.util.element.get_psets(element)

    def test_adding_a_pset_to_a_type_object(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Pset_WallCommon")
        assert pset.is_a("IfcPropertySet")
        assert "Pset_WallCommon" in ifcopenshell.util.element.get_psets(element)

    def test_adding_a_pset_to_a_context(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Custom_Pset")
        assert pset.is_a("IfcPropertySet")
        assert "Custom_Pset" in ifcopenshell.util.element.get_psets(element)

    def test_dedup_existing_pset_on_object(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset1 = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        pset2 = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        assert pset1 == pset2

    def test_dedup_existing_pset_on_type(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        pset1 = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        pset2 = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        assert pset1 == pset2


class TestAddPsetIFC2X3(test.bootstrap.IFC2X3, TestAddPset):
    pass
