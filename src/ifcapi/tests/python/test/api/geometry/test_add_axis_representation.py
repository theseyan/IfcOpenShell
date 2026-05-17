# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import test.bootstrap


class TestAddAxisRepresentation(test.bootstrap.IFC4):
    def setup_context(self, context_type="Plan", context_identifier="Axis"):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type=context_type)
        return ifcopenshell.api.context.add_context(
            self.file,
            context_type=context_type,
            context_identifier=context_identifier,
            target_view="GRAPH_VIEW",
            parent=parent,
        )

    def test_creates_2d_indexed_polycurve_axis(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_axis_representation(self.file, context, [(0.0, 0.0), (1.0, 0.0)])
        curve = rep.Items[0]
        assert rep.is_a("IfcShapeRepresentation")
        assert rep.ContextOfItems == context
        assert rep.RepresentationIdentifier == "Axis"
        assert rep.RepresentationType == "Curve2D"
        assert curve.is_a("IfcIndexedPolyCurve")
        assert curve.Points.is_a("IfcCartesianPointList2D")
        assert curve.Points.CoordList == ((0.0, 0.0), (1.0, 0.0))
        assert curve.SelfIntersect is False

    def test_creates_3d_indexed_polycurve_axis(self):
        context = self.setup_context(context_type="Model", context_identifier="Axis")
        rep = ifcopenshell.api.geometry.add_axis_representation(
            self.file, context, [(0.0, 0.0, 0.0), (0.0, 0.0, 1.0)]
        )
        curve = rep.Items[0]
        assert rep.RepresentationType == "Curve3D"
        assert curve.Points.is_a("IfcCartesianPointList3D")
        assert curve.Points.CoordList == ((0.0, 0.0, 0.0), (0.0, 0.0, 1.0))

    def test_empty_axis_is_an_error(self):
        context = self.setup_context()
        with pytest.raises(RuntimeError):
            ifcopenshell.api.geometry.add_axis_representation(self.file, context, [])


class TestAddAxisRepresentationIFC2X3(test.bootstrap.IFC2X3, TestAddAxisRepresentation):
    def test_creates_2d_indexed_polycurve_axis(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_axis_representation(self.file, context, [(0.0, 0.0), (1.0, 0.0)])
        curve = rep.Items[0]
        assert rep.RepresentationType == "Curve2D"
        assert curve.is_a("IfcPolyline")
        assert [point.Coordinates for point in curve.Points] == [(0.0, 0.0), (1.0, 0.0)]

    def test_creates_3d_indexed_polycurve_axis(self):
        context = self.setup_context(context_type="Model", context_identifier="Axis")
        rep = ifcopenshell.api.geometry.add_axis_representation(
            self.file, context, [(0.0, 0.0, 0.0), (0.0, 0.0, 1.0)]
        )
        curve = rep.Items[0]
        assert rep.RepresentationType == "Curve3D"
        assert curve.is_a("IfcPolyline")
        assert [point.Coordinates for point in curve.Points] == [(0.0, 0.0, 0.0), (0.0, 0.0, 1.0)]
