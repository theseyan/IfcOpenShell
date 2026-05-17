# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import test.bootstrap


class TestAddFootprintRepresentation(test.bootstrap.IFC4):
    def test_creates_geometric_curve_set_representation(self):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Plan")
        context = ifcopenshell.api.context.add_context(
            self.file,
            context_type="Plan",
            context_identifier="FootPrint",
            target_view="PLAN_VIEW",
            parent=parent,
        )
        curve = self.file.createIfcPolyline(
            [
                self.file.createIfcCartesianPoint((0.0, 0.0)),
                self.file.createIfcCartesianPoint((1.0, 0.0)),
            ]
        )
        rep = ifcopenshell.api.geometry.add_footprint_representation(self.file, context, [curve])
        curve_set = rep.Items[0]
        assert rep.is_a("IfcShapeRepresentation")
        assert rep.ContextOfItems == context
        assert rep.RepresentationIdentifier == "FootPrint"
        assert rep.RepresentationType == "GeometricCurveSet"
        assert curve_set.is_a("IfcGeometricCurveSet")
        assert curve_set.Elements == (curve,)


class TestAddFootprintRepresentationIFC2X3(test.bootstrap.IFC2X3, TestAddFootprintRepresentation):
    pass
