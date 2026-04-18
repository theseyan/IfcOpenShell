# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.drawing
import test.bootstrap


class TestAssignProduct(test.bootstrap.IFC4):
    def test_assigning_a_product(self):
        wall = self.file.createIfcWall()
        label = self.file.createIfcAnnotation()
        label2 = self.file.createIfcAnnotation()
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=wall, related_object=label)
        assert wall.ReferencedBy[0].RelatedObjects == (label,)
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=wall, related_object=label2)
        assert wall.ReferencedBy[0].RelatedObjects == (label, label2)

    def test_not_assigning_twice(self):
        wall = self.file.createIfcWall()
        label = self.file.createIfcAnnotation()
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=wall, related_object=label)
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=wall, related_object=label)
        assert len(wall.ReferencedBy) == 1
        assert wall.ReferencedBy[0].RelatedObjects == (label,)

    def test_assigning_a_grid_axis(self):
        axis = self.file.createIfcGridAxis(AxisTag="A")
        grid = self.file.createIfcGrid(UAxes=[axis])
        line = self.file.createIfcAnnotation()
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=axis, related_object=line)
        assert grid.ReferencedBy[0].RelatedObjects == (line,)
        assert grid.ReferencedBy[0].Name == "A"
        assert len(self.file.by_type("IfcRelAssignsToProduct")) == 1
        ifcopenshell.api.drawing.assign_product(self.file, relating_product=axis, related_object=line)
        assert len(self.file.by_type("IfcRelAssignsToProduct")) == 1


class TestAssignProductIFC2X3(test.bootstrap.IFC2X3, TestAssignProduct):
    pass
