# This file was generated with the assistance of an AI coding tool.

import pytest

import ifcopenshell.api.geometry
import test.bootstrap


@pytest.mark.skip(reason="C API file object does not support to_delete attribute")
class TestRemoveRepresentation(test.bootstrap.IFC4):
    def test_removing_a_single_unused_shape_representation(self):
        representation = self.file.createIfcShapeRepresentation()
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcShapeRepresentation")) == 0

    def test_not_removing_a_shape_representation_in_use(self):
        representation = self.file.createIfcShapeRepresentation()
        self.file.createIfcProductRepresentation(Representations=[representation])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcShapeRepresentation")) == 1

    def test_removing_a_mapped_representation_fully(self):
        representation = self.file.createIfcShapeRepresentation(
            RepresentationType="MappedRepresentation",
            Items=[
                self.file.createIfcMappedItem(
                    MappingTarget=self.file.createIfcRepresentationMap(
                        MappedRepresentation=self.file.createIfcShapeRepresentation()
                    )
                )
            ],
        )
        assert len(self.file.by_type("IfcShapeRepresentation")) == 2
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcShapeRepresentation")) == 0

    def test_removing_only_the_representation_mapping_if_the_map_has_other_users(self):
        representation_map = self.file.createIfcRepresentationMap(
            MappedRepresentation=self.file.createIfcShapeRepresentation()
        )
        self.file.createIfcWallType(RepresentationMaps=[representation_map])
        representation = self.file.createIfcShapeRepresentation(
            RepresentationType="MappedRepresentation",
            Items=[self.file.createIfcMappedItem(MappingTarget=representation_map)],
        )
        assert len(self.file.by_type("IfcShapeRepresentation")) == 2
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcShapeRepresentation")) == 1
        assert self.file.by_type("IfcShapeRepresentation")[0].RepresentationType != "MappedRepresentation"
        assert len(self.file.by_type("IfcRepresentationMap")) == 1

    def test_purging_styled_items_assignments_but_keeping_the_surface_style(self):
        item = self.file.createIfcExtrudedAreaSolid()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        surface_style = self.file.createIfcSurfaceStyle()
        styled_item = self.file.createIfcStyledItem(Item=item, Styles=[surface_style])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcStyledItem")) == 0
        assert len(self.file.by_type("IfcSurfaceStyle")) == 1

    def test_not_purging_styled_items_if_used_elsewhere(self):
        item = self.file.createIfcExtrudedAreaSolid()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        styled_item = self.file.createIfcStyledItem(Item=item)
        representation2 = self.file.createIfcShapeRepresentation(Items=[item])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcStyledItem")) == 1

    def test_purging_representation_presentation_layers(self):
        representation = self.file.createIfcShapeRepresentation()
        layer = self.file.createIfcPresentationLayerAssignment(AssignedItems=[representation])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcPresentationLayerAssignment")) == 0
        assert len(self.file.by_type("IfcShapeRepresentation")) == 0

    def test_not_purging_representation_presentation_layers_still_in_use(self):
        representation = self.file.createIfcShapeRepresentation()
        representation2 = self.file.createIfcShapeRepresentation()
        layer = self.file.createIfcPresentationLayerAssignment(AssignedItems=[representation, representation2])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcPresentationLayerAssignment")) == 1
        assert len(self.file.by_type("IfcShapeRepresentation")) == 1

    def test_purging_representation_item_presentation_layers(self):
        item = self.file.createIfcExtrudedAreaSolid()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        layer = self.file.createIfcPresentationLayerAssignment(AssignedItems=[item])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcPresentationLayerAssignment")) == 0
        assert len(self.file.by_type("IfcExtrudedAreaSolid")) == 0

    def test_not_purging_representation_item_presentation_layers_still_in_use(self):
        item, item2 = self.file.createIfcExtrudedAreaSolid(), self.file.createIfcExtrudedAreaSolid()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        representation2 = self.file.createIfcShapeRepresentation(Items=[item2])
        layer = self.file.createIfcPresentationLayerAssignment(AssignedItems=[item, item2])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcPresentationLayerAssignment")) == 1
        assert len(self.file.by_type("IfcExtrudedAreaSolid")) == 1

    def test_not_purging_geometric_representation_contexts(self):
        context = self.file.createIfcGeometricRepresentationSubContext()
        representation = self.file.createIfcShapeRepresentation(ContextOfItems=context)
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcGeometricRepresentationContext")) == 1

    def test_purging_colour_map(self):
        item = self.file.createIfcTriangulatedFaceSet()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        colour = self.file.createIfcIndexedColourMap(Colours=self.file.createIfcColourRgbList(), MappedTo=item)
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcIndexedColourMap")) == 0
        assert len(self.file.by_type("IfcColourRgbList")) == 0

    def test_purging_texture_coordinates(self):
        item = self.file.createIfcTriangulatedFaceSet()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        image = self.file.createIfcImageTexture()
        texture = self.file.createIfcIndexedTriangleTextureMap(
            TexCoords=self.file.createIfcTextureVertexList(), MappedTo=item, Maps=[image]
        )
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcIndexedTriangleTextureMap")) == 0
        assert len(self.file.by_type("IfcTextureVertexList")) == 0
        assert len(self.file.by_type("IfcImageTexture")) == 0

    def test_purging_texture_coordinates_but_not_images_used_in_other_styles(self):
        item = self.file.createIfcTriangulatedFaceSet()
        representation = self.file.createIfcShapeRepresentation(Items=[item])
        image = self.file.createIfcImageTexture()
        texture = self.file.createIfcIndexedTriangleTextureMap(
            TexCoords=self.file.createIfcTextureVertexList(), MappedTo=item, Maps=[image]
        )
        texture2 = self.file.createIfcIndexedTriangleTextureMap(Maps=[image])
        ifcopenshell.api.geometry.remove_representation(self.file, representation=representation)
        assert len(self.file.by_type("IfcIndexedTriangleTextureMap")) == 1
        assert len(self.file.by_type("IfcTextureVertexList")) == 0
        assert len(self.file.by_type("IfcImageTexture")) == 1
