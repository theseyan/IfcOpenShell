# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.grid
import test.bootstrap


class TestRemoveGridAxis(test.bootstrap.IFC4):
    def test_removing_an_axis_removes_its_curve(self):
        grid = self.file.createIfcGrid()
        axis = ifcopenshell.api.grid.create_grid_axis(
            self.file, axis_tag="A", same_sense=True, uvw_axes="UAxes", grid=grid
        )
        axis.AxisCurve = self.file.createIfcPolyline([self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))])
        axis2 = ifcopenshell.api.grid.create_grid_axis(
            self.file, axis_tag="B", same_sense=True, uvw_axes="UAxes", grid=grid
        )
        axis2.AxisCurve = self.file.createIfcPolyline([self.file.createIfcCartesianPoint((1.0, 0.0, 0.0))])
        ifcopenshell.api.grid.remove_grid_axis(self.file, axis=axis2)
        assert grid.UAxes == (axis,)
        assert len(self.file.by_type("IfcGridAxis")) == 1
        assert len(self.file.by_type("IfcPolyline")) == 1

    def test_removing_an_axis_preserves_shared_curve(self):
        grid = self.file.createIfcGrid()
        shared_curve = self.file.createIfcPolyline([self.file.createIfcCartesianPoint((0.0, 0.0, 0.0))])
        axis = ifcopenshell.api.grid.create_grid_axis(
            self.file, axis_tag="A", same_sense=True, uvw_axes="UAxes", grid=grid
        )
        axis.AxisCurve = shared_curve
        axis2 = ifcopenshell.api.grid.create_grid_axis(
            self.file, axis_tag="B", same_sense=True, uvw_axes="UAxes", grid=grid
        )
        axis2.AxisCurve = shared_curve
        ifcopenshell.api.grid.remove_grid_axis(self.file, axis=axis2)
        assert grid.UAxes == (axis,)
        assert shared_curve in self.file
        assert axis.AxisCurve == shared_curve


class TestRemoveGridAxisIFC2X3(test.bootstrap.IFC2X3, TestRemoveGridAxis):
    pass
