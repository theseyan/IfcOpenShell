# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for material.unassign_material — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.material
import ifcopenshell.api.root
import ifcopenshell.api.type
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignMaterialIFC2X3(test.bootstrap.IFC2X3):
    def test_unassign_single_material(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        mat = ifcopenshell.api.material.add_material(self.file, name="CON01")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterial", material=mat)
        ifcopenshell.api.material.unassign_material(self.file, products=[e1, e2])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 0
        assert len(self.file.by_type("IfcWall")) == 2
        assert len(self.file.by_type("IfcMaterial")) == 1

    def test_unassign_from_subset(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        mat = ifcopenshell.api.material.add_material(self.file, name="CON01")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterial", material=mat)
        ifcopenshell.api.material.unassign_material(self.file, products=[e2])
        assert e1.HasAssociations
        assert not e2.HasAssociations
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1

    def test_unassign_layer_set_from_type(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        t2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.material.assign_material(self.file, products=[t1, t2], type="IfcMaterialLayerSet")
        ifcopenshell.api.material.unassign_material(self.file, products=[t1, t2])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 0
        assert len(self.file.by_type("IfcMaterialLayerSet")) == 1

    def test_unassign_layer_set_usage(self):
        et = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[e1, e2], relating_type=et)
        ifcopenshell.api.material.assign_material(self.file, products=[et], type="IfcMaterialLayerSet")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterialLayerSetUsage")
        ifcopenshell.api.material.unassign_material(self.file, products=[e1, e2])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        assert et.HasAssociations
        assert len(self.file.by_type("IfcMaterialLayerSetUsage")) == 0

    def test_unassign_material_list(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        mat = ifcopenshell.api.material.add_material(self.file, name="CON01")
        ifcopenshell.api.material.assign_material(self.file, products=[e1], type="IfcMaterialList", material=mat)
        ifcopenshell.api.material.unassign_material(self.file, products=[e1])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 0
        assert len(self.file.by_type("IfcMaterialList")) == 1


class TestUnassignMaterialIFC4(test.bootstrap.IFC4, TestUnassignMaterialIFC2X3):
    def test_unassign_profile_set_from_type(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.material.assign_material(self.file, products=[t1], type="IfcMaterialProfileSet")
        ifcopenshell.api.material.unassign_material(self.file, products=[t1])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 0
        assert len(self.file.by_type("IfcMaterialProfileSet")) == 1

    def test_unassign_profile_set_usage(self):
        et = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[e1, e2], relating_type=et)
        ifcopenshell.api.material.assign_material(self.file, products=[et], type="IfcMaterialProfileSet")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterialProfileSetUsage")
        ifcopenshell.api.material.unassign_material(self.file, products=[e1, e2])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        assert et.HasAssociations
        assert len(self.file.by_type("IfcMaterialProfileSetUsage")) == 0

    def test_unassign_constituent_set(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        t2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.material.assign_material(self.file, products=[t1, t2], type="IfcMaterialConstituentSet")
        ifcopenshell.api.material.unassign_material(self.file, products=[t1, t2])
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 0
        assert len(self.file.by_type("IfcMaterialConstituentSet")) == 1
