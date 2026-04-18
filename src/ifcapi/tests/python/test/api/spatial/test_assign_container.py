# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.aggregate
import ifcopenshell.api.root
import ifcopenshell.api.spatial
import ifcopenshell.util.element
import test.bootstrap


class TestAssignContainer(test.bootstrap.IFC4):
    def test_assigning_a_container(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        rel = ifcopenshell.api.spatial.assign_container(
            self.file, products=[subelement, subelement2], relating_structure=element
        )
        assert ifcopenshell.util.element.get_container(subelement) == element
        assert ifcopenshell.util.element.get_container(subelement2) == element
        assert rel.is_a("IfcRelContainedInSpatialStructure")

    def test_doing_nothing_if_the_container_is_already_assigned(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement], relating_structure=element)
        total_elements = len([e for e in self.file])
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement], relating_structure=element)
        assert len([e for e in self.file]) == total_elements

    def test_that_old_containment_relationships_are_updated_if_they_still_contain_elements(self):
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement1], relating_structure=element1)
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement2], relating_structure=element1)
        rel = subelement1.ContainedInStructure[0]
        assert len(rel.RelatedElements) == 2
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement1], relating_structure=element2)
        assert len(rel.RelatedElements) == 1

    def test_that_old_containment_relationships_are_purged_if_no_more_elements_are_contained(self):
        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement1], relating_structure=element1)
        rel_id = subelement1.ContainedInStructure[0].id()
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement1], relating_structure=element2)
        with pytest.raises(RuntimeError):
            self.file.by_id(rel_id)

    def test_removing_aggregation_if_it_exists(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        aggregate = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcElementAssembly")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.aggregate.assign_object(self.file, products=[subelement], relating_object=aggregate)
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement], relating_structure=element)
        assert not ifcopenshell.util.element.get_aggregate(subelement)


class TestAssignContainerIFC2X3(test.bootstrap.IFC2X3, TestAssignContainer):
    pass
