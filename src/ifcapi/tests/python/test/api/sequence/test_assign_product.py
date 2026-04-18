# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.sequence
import test.bootstrap


class TestAssignProduct(test.bootstrap.IFC4):
    def test_assigning_a_product(self):
        wall = self.file.createIfcWall()
        task = self.file.createIfcTask()
        task2 = self.file.createIfcTask()
        ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task)
        assert wall.ReferencedBy[0].RelatedObjects == (task,)
        ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task2)
        assert wall.ReferencedBy[0].RelatedObjects == (task, task2)

    def test_not_assigning_twice(self):
        wall = self.file.createIfcWall()
        task = self.file.createIfcTask()
        ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task)
        ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task)
        assert wall.ReferencedBy[0].RelatedObjects == (task,)


class TestAssignProductIFC2X3(test.bootstrap.IFC2X3, TestAssignProduct):
    pass
