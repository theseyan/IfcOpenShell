# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for material.remove_material — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.material
import ifcopenshell.api.root
import test.bootstrap


class TestRemoveMaterial(test.bootstrap.IFC4):
    def test_removing_material(self):
        mat = ifcopenshell.api.material.add_material(self.file)
        ifcopenshell.api.material.remove_material(self.file, material=mat)
        assert len(self.file.by_type("IfcMaterial")) == 0

    def test_removing_material_with_associations(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        mat = ifcopenshell.api.material.add_material(self.file)
        ifcopenshell.api.material.assign_material(self.file, products=[wall], material=mat)
        ifcopenshell.api.material.remove_material(self.file, material=mat)
        assert len(self.file.by_type("IfcMaterial")) == 0
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 0


class TestRemoveMaterialIFC2X3(test.bootstrap.IFC2X3, TestRemoveMaterial):
    pass
