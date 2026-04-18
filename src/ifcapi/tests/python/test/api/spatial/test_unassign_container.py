# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root
import ifcopenshell.api.spatial
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignContainer(test.bootstrap.IFC4):
    def test_unassigning_a_container(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(
            self.file, products=[subelement, subelement2], relating_structure=element
        )
        ifcopenshell.api.spatial.unassign_container(self.file, products=[subelement, subelement2])
        assert not self.file.by_type("IfcRelContainedInSpatialStructure")

    def test_doing_nothing_if_no_container(self):
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.unassign_container(self.file, products=[subelement])
        assert ifcopenshell.util.element.get_container(subelement) is None

    def test_updating_the_rel_when_a_container_is_removed_with_multiple_elements(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(
            self.file, products=[subelement, subelement2], relating_structure=element
        )
        ifcopenshell.api.spatial.unassign_container(self.file, products=[subelement])
        rel = self.file.by_type("IfcRelContainedInSpatialStructure")[0]
        assert list(rel.RelatedElements) == [subelement2]

    def test_deleting_the_rel_when_a_container_is_removed_with_no_elements(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcBuilding")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.spatial.assign_container(self.file, products=[subelement], relating_structure=element)
        ifcopenshell.api.spatial.unassign_container(self.file, products=[subelement])
        assert len(self.file.by_type("IfcRelContainedInSpatialStructure")) == 0


class TestUnassignContainerIFC2X3(test.bootstrap.IFC2X3, TestUnassignContainer):
    pass
