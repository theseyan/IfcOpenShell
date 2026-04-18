# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for pset.unassign_pset — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignPset(test.bootstrap.IFC4):
    def test_unassign_from_last_occurrence(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=e1, name="Foo")
        ifcopenshell.api.pset.unassign_pset(self.file, [e1], pset)
        # Rel should be removed
        assert len(self.file.by_type("IfcRelDefinesByProperties")) == 0

    def test_unassign_from_non_last_occurrence(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=e1, name="Foo")
        ifcopenshell.api.pset.assign_pset(self.file, [e2], pset)
        ifcopenshell.api.pset.unassign_pset(self.file, [e1], pset)
        # Rel should still exist with e2
        rels = self.file.by_type("IfcRelDefinesByProperties")
        assert len(rels) == 1
        assert e2 in rels[0].RelatedObjects
        assert e1 not in rels[0].RelatedObjects

    def test_unassign_from_type(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=t1, name="Foo")
        assert pset in (t1.HasPropertySets or ())
        ifcopenshell.api.pset.unassign_pset(self.file, [t1], pset)
        assert t1.HasPropertySets is None or pset not in t1.HasPropertySets


class TestUnassignPsetIFC2X3(test.bootstrap.IFC2X3, TestUnassignPset):
    pass
