# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.aggregate
import ifcopenshell.api.root
import ifcopenshell.api.spatial
import ifcopenshell.util.element
import test.bootstrap


class TestAssignObject(test.bootstrap.IFC4):
    def test_assigning_an_aggregate(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSite")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        rel = ifcopenshell.api.aggregate.assign_object(
            self.file, products=[subelement1, subelement2], relating_object=element
        )
        assert ifcopenshell.util.element.get_aggregate(subelement1) == element
        assert ifcopenshell.util.element.get_aggregate(subelement2) == element
        assert rel.is_a("IfcRelAggregates")

    def test_doing_nothing_if_the_aggregate_is_already_assigned(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSite")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement], relating_object=element)
        total_elements = len([e for e in self.file])
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement], relating_object=element)
        assert len([e for e in self.file]) == total_elements

    def test_that_old_aggregate_relationships_are_updated_if_they_still_have_elements(self):
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSite")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSite")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement1], relating_object=element1)
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement2], relating_object=element1)
        rel = subelement1.Decomposes[0]
        assert len(rel.RelatedObjects) == 2
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement1], relating_object=element2)
        assert len(rel.RelatedObjects) == 1

    def test_that_old_aggregate_relationships_are_purged_if_no_more_elements_are_contained(self):
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSite")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSite")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement1], relating_object=element1)
        rel_id = subelement1.Decomposes[0].id()
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement1], relating_object=element2)
        with pytest.raises(RuntimeError):
            self.file.by_id(rel_id)

    def test_removing_containment_if_it_exists(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcElementAssembly")
        container = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuildingStorey")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement], relating_structure=container)
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement], relating_object=element)
        assert not ifcopenshell.util.element.get_container(subelement, should_get_direct=True)


class TestAssignObjectIFC2X3(test.bootstrap.IFC2X3, TestAssignObject):
    pass
