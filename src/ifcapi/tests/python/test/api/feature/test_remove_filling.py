
import pytest

import ifcopenshell.api.feature
import ifcopenshell.api.root
import test.bootstrap


class TestRemoveFilling(test.bootstrap.IFC4):
    def test_removing_a_filling(self):
        opening = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcOpeningElement")
        door = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcDoor")
        ifcopenshell.api.feature.add_filling(self.file, opening=opening, element=door)
        ifcopenshell.api.feature.remove_filling(self.file, element=door)
        assert len(self.file.by_type("IfcRelFillsElement")) == 0
        assert opening
        assert door

    def test_removing_a_filling_that_does_not_exist(self):
        door = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcDoor")
        ifcopenshell.api.feature.remove_filling(self.file, element=door)
        assert len(self.file.by_type("IfcRelFillsElement")) == 0


class TestRemoveFillingIFC2X3(test.bootstrap.IFC2X3, TestRemoveFilling):
    pass
