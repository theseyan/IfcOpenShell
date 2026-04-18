# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for material.assign_material — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.material
import ifcopenshell.api.root
import ifcopenshell.api.type
import ifcopenshell.util.element
import test.bootstrap


class TestAssignMaterialIFC2X3(test.bootstrap.IFC2X3):
    def test_assign_element_single_material(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        mat = ifcopenshell.api.material.add_material(self.file, name="CON01")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterial", material=mat)
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        assert ifcopenshell.util.element.get_material(e1) == mat
        assert ifcopenshell.util.element.get_material(e2) == mat

    def test_assign_element_material_no_material(self):
        e = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.material.assign_material(self.file, products=[e], type="IfcMaterial", material=None)
        assert ifcopenshell.util.element.get_material(e)

    def test_assign_type_single_material(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        t2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        mat = ifcopenshell.api.material.add_material(self.file, name="CON01")
        ifcopenshell.api.material.assign_material(self.file, products=[t1, t2], type="IfcMaterial", material=mat)
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        assert ifcopenshell.util.element.get_material(t1) == mat
        assert ifcopenshell.util.element.get_material(t2) == mat

    def test_assign_type_material_layer_set(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        t2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.material.assign_material(self.file, products=[t1, t2], type="IfcMaterialLayerSet")
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        ms = ifcopenshell.util.element.get_material(t1)
        assert ms.is_a("IfcMaterialLayerSet")
        assert not ms.MaterialLayers
        assert ifcopenshell.util.element.get_material(t2) == ms

    def test_assign_layer_set_usage(self):
        et = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[e1, e2], relating_type=et)
        ifcopenshell.api.material.assign_material(self.file, products=[et], type="IfcMaterialLayerSet")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterialLayerSetUsage")
        ms = ifcopenshell.util.element.get_material(et)
        mu = ifcopenshell.util.element.get_material(e1, should_inherit=False)
        assert mu.is_a("IfcMaterialLayerSetUsage")
        assert mu.ForLayerSet == ms
        assert ifcopenshell.util.element.get_material(e2, should_inherit=False) == mu

    def test_layer_set_direction_varies(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterialLayerSetUsage")
        mu1 = ifcopenshell.util.element.get_material(e1, should_inherit=False)
        mu2 = ifcopenshell.util.element.get_material(e2, should_inherit=False)
        assert mu1.LayerSetDirection == "AXIS2"
        assert mu2.LayerSetDirection == "AXIS3"
        assert mu1 != mu2

    def test_assign_material_list(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        mat = ifcopenshell.api.material.add_material(self.file, name="CON01")
        ifcopenshell.api.material.assign_material(self.file, products=[e1], type="IfcMaterialList", material=mat)
        ml = ifcopenshell.util.element.get_material(e1)
        assert ml.is_a("IfcMaterialList")
        assert ml.Materials[0] == mat


class TestAssignMaterialIFC4(test.bootstrap.IFC4, TestAssignMaterialIFC2X3):
    def test_assign_type_material_profile_set(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        t2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.material.assign_material(self.file, products=[t1, t2], type="IfcMaterialProfileSet")
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        ms = ifcopenshell.util.element.get_material(t1)
        assert ms.is_a("IfcMaterialProfileSet")
        assert not ms.MaterialProfiles
        assert ifcopenshell.util.element.get_material(t2) == ms

    def test_assign_profile_set_usage(self):
        et = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(self.file, related_objects=[e1, e2], relating_type=et)
        ifcopenshell.api.material.assign_material(self.file, products=[et], type="IfcMaterialProfileSet")
        ifcopenshell.api.material.assign_material(self.file, products=[e1, e2], type="IfcMaterialProfileSetUsage")
        ms = et.HasAssociations[0].RelatingMaterial
        mu = e1.HasAssociations[0].RelatingMaterial
        assert mu.is_a("IfcMaterialProfileSetUsage")
        assert mu.ForProfileSet == ms
        assert ifcopenshell.util.element.get_material(e2, should_inherit=False) == mu

    def test_assign_type_material_constituent_set(self):
        t1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        t2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWallType")
        ifcopenshell.api.material.assign_material(self.file, products=[t1, t2], type="IfcMaterialConstituentSet")
        assert len(self.file.by_type("IfcRelAssociatesMaterial")) == 1
        ms = ifcopenshell.util.element.get_material(t1)
        assert ms.is_a("IfcMaterialConstituentSet")
        assert not ms.MaterialConstituents
        assert ifcopenshell.util.element.get_material(t2) == ms
