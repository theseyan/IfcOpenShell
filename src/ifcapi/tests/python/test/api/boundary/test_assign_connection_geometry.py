# This file was generated with the assistance of an AI coding tool.

import pytest

import ifcopenshell.api.boundary
import ifcopenshell.api.root
import test.bootstrap


class TestAssignConnectionGeometry(test.bootstrap.IFC4):
    def test_assigns_curve_bounded_plane_geometry(self):
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")

        ifcopenshell.api.boundary.assign_connection_geometry(
            self.file,
            rel_space_boundary=boundary,
            outer_boundary=[(0.0, 0.0), (2.0, 0.0), (2.0, 2.0), (0.0, 2.0), (0.0, 0.0)],
            inner_boundaries=[[(0.5, 0.5), (1.5, 0.5), (1.5, 1.5), (0.5, 1.5)]],
            location=(10.0, 20.0, 30.0),
            axis=(1.0, 0.0, 0.0),
            ref_direction=(0.0, 0.0, 1.0),
            unit_scale=0.5,
        )

        geometry = boundary.ConnectionGeometry
        assert geometry.is_a("IfcConnectionSurfaceGeometry")
        plane = geometry.SurfaceOnRelatingElement
        assert plane.is_a("IfcCurveBoundedPlane")
        assert plane.BasisSurface.Position.Location.Coordinates == (20.0, 40.0, 60.0)
        assert plane.BasisSurface.Position.Axis.DirectionRatios == (1.0, 0.0, 0.0)
        assert plane.BasisSurface.Position.RefDirection.DirectionRatios == (0.0, 0.0, 1.0)
        assert [p.Coordinates for p in plane.OuterBoundary.Points] == [
            (0.0, 0.0),
            (4.0, 0.0),
            (4.0, 4.0),
            (0.0, 4.0),
            (0.0, 0.0),
        ]
        assert len(plane.InnerBoundaries) == 1
        assert [p.Coordinates for p in plane.InnerBoundaries[0].Points] == [
            (1.0, 1.0),
            (3.0, 1.0),
            (3.0, 3.0),
            (1.0, 3.0),
            (1.0, 1.0),
        ]

    def test_uses_numpy_allclose_semantics_when_stripping_closing_point(self):
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")

        ifcopenshell.api.boundary.assign_connection_geometry(
            self.file,
            rel_space_boundary=boundary,
            outer_boundary=[(0.0, 0.0), (1.0, 0.0), (1.0, 1.0), (1e-9, 1e-9)],
            location=(0.0, 0.0, 0.0),
            axis=(1.0, 0.0, 0.0),
            ref_direction=(0.0, 0.0, 1.0),
            unit_scale=1.0,
        )

        assert [p.Coordinates for p in boundary.ConnectionGeometry.SurfaceOnRelatingElement.OuterBoundary.Points] == [
            (0.0, 0.0),
            (1.0, 0.0),
            (1.0, 1.0),
            (0.0, 0.0),
        ]

    def test_rejects_empty_boundaries_without_creating_geometry(self):
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")

        with pytest.raises(RuntimeError, match="polyline must contain"):
            ifcopenshell.api.boundary.assign_connection_geometry(
                self.file,
                rel_space_boundary=boundary,
                outer_boundary=[],
                location=(0.0, 0.0, 0.0),
                axis=(1.0, 0.0, 0.0),
                ref_direction=(0.0, 0.0, 1.0),
                unit_scale=1.0,
            )

        assert boundary.ConnectionGeometry is None


class TestAssignConnectionGeometryIFC2X3(test.bootstrap.IFC2X3, TestAssignConnectionGeometry):
    pass
