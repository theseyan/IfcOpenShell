# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.root
import ifcopenshell.api.type
import ifcopenshell.util.element
import test.bootstrap


class TestAssignType(test.bootstrap.IFC4):
    def test_assigning_a_type(self):
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element_type = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        rel = ifcopenshell.api.type.assign_type(
            self.file, related_objects=[element1, element2], relating_type=element_type
        )
        assert ifcopenshell.util.element.get_type(element1) == element_type
        assert ifcopenshell.util.element.get_type(element2) == element_type
        assert rel.is_a("IfcRelDefinesByType")

    def test_doing_nothing_if_type_is_already_assigned(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element_type = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element], relating_type=element_type)
        total_elements = len([e for e in self.file])
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element], relating_type=element_type)
        assert len([e for e in self.file]) == total_elements

    def test_that_old_typing_relationships_are_updated_if_they_still_have_elements(self):
        element_type1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element_type2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element1, element2], relating_type=element_type1)
        rel = element1.IsTypedBy[0] if self.file.schema != "IFC2X3" else next(
            r for r in element1.IsDefinedBy if r.is_a("IfcRelDefinesByType")
        )
        assert len(rel.RelatedObjects) == 2
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element1], relating_type=element_type2)
        assert len(rel.RelatedObjects) == 1

    def test_that_old_typing_relationships_are_purged_if_no_more_elements(self):
        element_type1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element_type2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element1], relating_type=element_type1)
        rel = element1.IsTypedBy[0] if self.file.schema != "IFC2X3" else next(
            r for r in element1.IsDefinedBy if r.is_a("IfcRelDefinesByType")
        )
        rel_id = rel.id()
        ifcopenshell.api.type.assign_type(self.file, related_objects=[element1], relating_type=element_type2)
        with pytest.raises(RuntimeError):
            self.file.by_id(rel_id)


class TestAssignTypeIFC2X3(test.bootstrap.IFC2X3, TestAssignType):
    pass
