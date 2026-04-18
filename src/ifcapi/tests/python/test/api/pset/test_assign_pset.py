# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for pset.assign_pset — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.pset
import test.bootstrap


class TestAssignPset(test.bootstrap.IFC4):
    def test_assign_pset_to_occurrence(self):
        elements = [self.file.create_entity("IfcWall") for _ in range(3)]
        pset = self.file.create_entity("IfcPropertySet")
        rel = ifcopenshell.api.pset.assign_pset(self.file, elements, pset)
        assert rel
        assert len(self.file.by_type("IfcRelDefinesByProperties")) == 1
        assert rel.RelatingPropertyDefinition == pset
        assert set(rel.RelatedObjects) == set(elements)

    def test_assign_pset_to_occurrence_preexisting_rel(self):
        elements = [self.file.create_entity("IfcWall") for _ in range(3)]
        pset = self.file.create_entity("IfcPropertySet")
        rel = ifcopenshell.api.pset.assign_pset(self.file, elements[:1], pset)
        assert rel
        rel_updated = ifcopenshell.api.pset.assign_pset(self.file, elements[1:], pset)
        assert rel_updated == rel
        assert len(self.file.by_type("IfcRelDefinesByProperties")) == 1
        assert set(rel.RelatedObjects) == set(elements)

    def test_assign_pset_to_type(self):
        elements = [self.file.create_entity("IfcWallType") for _ in range(3)]
        pset = self.file.create_entity("IfcPropertySet")
        ret = ifcopenshell.api.pset.assign_pset(self.file, elements, pset)
        assert ret is None
        assert len(self.file.by_type("IfcRelDefinesByProperties")) == 0
        assert set(pset.DefinesType) == set(elements)


class TestAssignPsetIFC2X3(test.bootstrap.IFC2X3, TestAssignPset):
    pass
