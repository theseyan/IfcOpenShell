# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root
import ifcopenshell.api.type
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignType(test.bootstrap.IFC4):
    def test_unassigning_a_type(self):
        element_type = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element1, element2], relating_type=element_type)
        ifcopenshell.api.type.unassign_type(self.file, related_objects=[element1, element2])
        assert ifcopenshell.util.element.get_type(element1) is None
        assert ifcopenshell.util.element.get_type(element2) is None

    def test_the_rel_is_kept_if_there_are_more_typed_elements(self):
        element_type = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element1], relating_type=element_type)
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element2], relating_type=element_type)
        ifcopenshell.api.type.unassign_type(self.file, related_objects=[element1])
        assert len(self.file.by_type("IfcRelDefinesByType")) == 1

    def test_the_rel_is_purged_if_there_are_no_more_typed_elements(self):
        element_type = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element], relating_type=element_type)
        ifcopenshell.api.type.unassign_type(self.file, related_objects=[element])
        assert len(self.file.by_type("IfcRelDefinesByType")) == 0


class TestUnassignTypeIFC2X3(test.bootstrap.IFC2X3, TestUnassignType):
    pass
