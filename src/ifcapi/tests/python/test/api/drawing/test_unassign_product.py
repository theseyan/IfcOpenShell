# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.drawing
import test.bootstrap


class TestUnassignProduct(test.bootstrap.IFC4):
    def test_unassigning_a_product(self):
        wall = self.file.createIfcWall()
        label = self.file.createIfcAnnotation()
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=wall, related_object=label)
        ifcopenshell.api.drawing.unassign_product(self.file, relating_product=wall, related_object=label)
        assert len(self.file.by_type("IfcRelAssignsToProduct")) == 0

    def test_unassigning_a_grid_axis(self):
        axis = self.file.createIfcGridAxis(AxisTag="A")
        grid = self.file.createIfcGrid(UAxes=[axis])
        line = self.file.createIfcAnnotation()
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=axis, related_object=line)
        ifcopenshell.api.drawing.unassign_product(self.file, relating_product=axis, related_object=line)
        assert len(self.file.by_type("IfcRelAssignsToProduct")) == 0


class TestUnassignProductIFC2X3(test.bootstrap.IFC2X3, TestUnassignProduct):
    pass
