# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/group/test_unassign_group.py

import ifcopenshell.api.group
import ifcopenshell.api.root
import test.bootstrap


class TestUnassignGroup(test.bootstrap.IFC4):
    def test_group_unassignment(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element3 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        group = ifcopenshell.api.group.add_group(self.file)
        ifcopenshell.api.group.assign_group(self.file, products=[element, element2, element3], group=group)
        ifcopenshell.api.group.unassign_group(self.file, products=[element2, element3], group=group)

        assert len(rels := self.file.by_type("IfcRelAssignsToGroup")) == 1
        rel = rels[0]
        assert rel.RelatingGroup == group
        assert rel.RelatedObjects == (element,)

    def test_remove_relationship_unassigning_last_element(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        group = ifcopenshell.api.group.add_group(self.file)
        ifcopenshell.api.group.assign_group(self.file, products=[element, element2], group=group)
        ifcopenshell.api.group.unassign_group(self.file, products=[element, element2], group=group)
        assert len(self.file.by_type("IfcRelAssignsToGroup")) == 0


class TestUnassignGroupIFC2X3(test.bootstrap.IFC2X3, TestUnassignGroup):
    pass
