# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/group/test_add_group.py

import ifcopenshell.api.group
import test.bootstrap


class TestAddGroup(test.bootstrap.IFC4):
    def test_add_group_no_arguments(self):
        group = ifcopenshell.api.group.add_group(self.file)
        assert group.Name == "Unnamed"
        assert group.Description is None

    def test_add_group(self):
        group = ifcopenshell.api.group.add_group(self.file, name="Name", description="Description")
        assert group.Name == "Name"
        assert group.Description == "Description"


class TestAddGroupIFC2X3(test.bootstrap.IFC2X3, TestAddGroup):
    pass
