# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import test.bootstrap


class TestAddMeshRepresentation(test.bootstrap.IFC4):
    def setup_context(self, context_identifier="Body"):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        if context_identifier is None:
            return parent
        return ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier=context_identifier,
            target_view="MODEL_VIEW",
            parent=parent,
        )

    def test_creates_polygonal_face_set_representation(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[[(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (1.0, 1.0, 0.0), (0.0, 1.0, 0.0)]],
            faces=[[(0, 1, 2, 3)]],
        )

        item = rep.Items[0]
        assert rep.is_a("IfcShapeRepresentation")
        assert rep.ContextOfItems == context
        assert rep.RepresentationIdentifier == "Body"
        assert rep.RepresentationType == "Tessellation"
        assert item.is_a("IfcPolygonalFaceSet")
        assert item.Coordinates.CoordList == ((0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (1.0, 1.0, 0.0), (0.0, 1.0, 0.0))
        assert item.Faces[0].CoordIndex == (1, 2, 3, 4)

    def test_preserves_null_context_identifier(self):
        context = self.setup_context(context_identifier=None)
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[[(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (0.0, 1.0, 0.0)]],
            faces=[[(0, 1, 2)]],
        )
        assert rep.RepresentationIdentifier is None

    def test_applies_unit_scale_and_coordinate_offset(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[[(1.0, 1.5, 2.0), (2.0, 1.5, 2.0), (1.0, 2.5, 2.0)]],
            faces=[[(0, 1, 2)]],
            unit_scale=0.5,
            coordinate_offset=(1.0, 2.0, 3.0),
        )
        assert rep.Items[0].Coordinates.CoordList == ((3.0, 5.0, 7.0), (5.0, 5.0, 7.0), (3.0, 7.0, 7.0))

    def test_creates_multiple_mesh_items(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[
                [(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (0.0, 1.0, 0.0)],
                [(0.0, 0.0, 1.0), (1.0, 0.0, 1.0), (0.0, 1.0, 1.0)],
            ],
            faces=[[(0, 1, 2)], [(0, 1, 2)]],
        )
        assert [item.is_a() for item in rep.Items] == ["IfcPolygonalFaceSet", "IfcPolygonalFaceSet"]

    def test_preserves_polygonal_face_voids(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[
                [
                    (0.0, 0.0, 0.0),
                    (3.0, 0.0, 0.0),
                    (3.0, 3.0, 0.0),
                    (0.0, 3.0, 0.0),
                    (1.0, 1.0, 0.0),
                    (2.0, 1.0, 0.0),
                    (2.0, 2.0, 0.0),
                    (1.0, 2.0, 0.0),
                ]
            ],
            faces=[[[[0, 1, 2, 3], [4, 5, 6, 7]]]],
        )
        face = rep.Items[0].Faces[0]
        assert face.is_a("IfcIndexedPolygonalFaceWithVoids")
        assert face.CoordIndex == (1, 2, 3, 4)
        assert face.InnerCoordIndices == ((5, 6, 7, 8),)

    def test_force_faceted_brep_creates_brep_representation(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[[(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (0.0, 1.0, 0.0)]],
            faces=[[(0, 1, 2)]],
            force_faceted_brep=True,
        )
        assert rep.RepresentationType == "Brep"
        assert rep.Items[0].is_a("IfcFacetedBrep")

    def test_preserves_upstream_assertions(self):
        context = self.setup_context()
        with pytest.raises(AssertionError):
            ifcopenshell.api.geometry.add_mesh_representation(self.file, context, vertices=[], faces=[])
        with pytest.raises(AssertionError):
            ifcopenshell.api.geometry.add_mesh_representation(self.file, context, vertices=[[]], faces=[])
        with pytest.raises(AssertionError):
            ifcopenshell.api.geometry.add_mesh_representation(self.file, context, vertices=[[]], faces=None)


class TestAddMeshRepresentationIFC2X3(test.bootstrap.IFC2X3):
    def setup_context(self):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        return ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Body",
            target_view="MODEL_VIEW",
            parent=parent,
        )

    def test_creates_faceted_brep_representation(self):
        context = self.setup_context()
        rep = ifcopenshell.api.geometry.add_mesh_representation(
            self.file,
            context,
            vertices=[[(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (0.0, 1.0, 0.0)]],
            faces=[[(0, 1, 2)]],
        )
        assert rep.RepresentationType == "Brep"
        assert rep.Items[0].is_a("IfcFacetedBrep")
