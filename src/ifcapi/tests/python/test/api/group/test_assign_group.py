# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/group/test_assign_group.py

import ifcopenshell.api.group
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestAssignGroup(test.bootstrap.IFC4):
    def test_assign_group_for_multiple_elements(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        group = ifcopenshell.api.group.add_group(self.file)

        ifcopenshell.api.group.assign_group(self.file, products=[element, element2], group=group)
        assert len(self.file.by_type("IfcRelAssignsToGroup")) == 1
        assert set(ifcopenshell.util.element.get_grouped_by(group)) == set(self.file.by_type("IfcWall"))

    def test_reuse_existing_relationship(self):
        self.test_assign_group_for_multiple_elements()
        rel = self.file.by_type("IfcRelAssignsToGroup")[0]
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        group = self.file.by_type("IfcGroup")[0]
        ifcopenshell.api.group.assign_group(self.file, products=[element], group=group)

        assert len(self.file.by_type("IfcRelAssignsToGroup")) == 1
        assert set(ifcopenshell.util.element.get_grouped_by(group)) == set(self.file.by_type("IfcWall"))


class TestAssignGroupIFC2X3(test.bootstrap.IFC2X3, TestAssignGroup):
    pass
