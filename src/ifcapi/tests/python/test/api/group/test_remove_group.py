# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/group/test_remove_group.py

import ifcopenshell.api.group
import ifcopenshell.api.root
import test.bootstrap


class TestRemoveGroup(test.bootstrap.IFC4):
    def test_removing_a_group(self):
        group = ifcopenshell.api.group.add_group(self.file)
        ifcopenshell.api.group.remove_group(self.file, group=group)
        assert len(self.file.by_type("IfcGroup")) == 0

    def test_removing_orphaned_group_relationships(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        group = ifcopenshell.api.group.add_group(self.file)
        ifcopenshell.api.group.assign_group(self.file, products=[element], group=group)
        ifcopenshell.api.group.remove_group(self.file, group=group)
        assert not self.file.by_type("IfcRelAssignsToGroup")

    # test_removing_orphaned_property_relationships skipped — requires pset API


class TestRemoveGroupIFC2X3(test.bootstrap.IFC2X3, TestRemoveGroup):
    pass
