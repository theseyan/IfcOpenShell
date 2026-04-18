# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.sequence
import test.bootstrap


class TestUnassignProduct(test.bootstrap.IFC4):
    def test_unassigning_a_product(self):
        wall = self.file.createIfcWall()
        task = self.file.createIfcTask()
        ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task)
        ifcopenshell.api.sequence.unassign_product(self.file, relating_product=wall, related_object=task)
        assert len(self.file.by_type("IfcRelAssignsToProduct")) == 0


class TestUnassignProductIFC2X3(test.bootstrap.IFC2X3, TestUnassignProduct):
    pass
