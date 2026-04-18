# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for attribute.edit_attributes."""

import ifcopenshell
import ifcopenshell.api.attribute
import ifcopenshell.api.root
import test.bootstrap


class TestEditAttributes(test.bootstrap.IFC4):
    def test_edit_name(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.attribute.edit_attributes(self.file, product=wall, attributes={"Name": "Foo"})
        assert wall.Name == "Foo"

    def test_edit_multiple(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.attribute.edit_attributes(
            self.file, product=wall, attributes={"Name": "Bar", "Description": "A wall"})
        assert wall.Name == "Bar"
        assert wall.Description == "A wall"

    def test_predefined_type_auto_userdefined(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        if not hasattr(wall, "PredefinedType"):
            return  # IFC2X3 IfcWall has no PredefinedType
        ifcopenshell.api.attribute.edit_attributes(
            self.file, product=wall, attributes={"ObjectType": "CustomWall"})
        assert wall.PredefinedType == "USERDEFINED"

    def test_predefined_type_notdefined_when_no_object_type(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        if not hasattr(wall, "PredefinedType"):
            return  # IFC2X3 IfcWall has no PredefinedType
        wall.PredefinedType = "USERDEFINED"
        wall.ObjectType = "SomeType"
        ifcopenshell.api.attribute.edit_attributes(
            self.file, product=wall, attributes={"ObjectType": None})
        assert wall.PredefinedType == "NOTDEFINED"


class TestEditAttributesIFC2X3(test.bootstrap.IFC2X3, TestEditAttributes):
    pass
