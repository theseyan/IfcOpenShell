# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.style
import test.bootstrap


class TestRemoveSurfaceStyleIFC2X3(test.bootstrap.IFC2X3):
    def test_removing_a_shading_style(self):
        style = self.file.createIfcSurfaceStyleShading(SurfaceColour=self.file.createIfcColourRgb(None, 1, 1, 1))
        ifcopenshell.api.style.remove_surface_style(self.file, style=style)
        assert len(list(self.file)) == 0

    def test_removing_a_texture_style(self):
        texture = self.file.createIfcImageTexture()
        style = self.file.createIfcSurfaceStyleWithTextures(Textures=[texture])
        ifcopenshell.api.style.remove_surface_style(self.file, style=style)
        assert len(list(self.file)) == 0


class TestRemoveSurfaceStyleIFC4(test.bootstrap.IFC4, TestRemoveSurfaceStyleIFC2X3):
    def test_removing_a_texture_style_with_all_of_its_coordinates(self):
        texture = self.file.createIfcImageTexture()
        coordinates = self.file.createIfcTextureCoordinateGenerator(Maps=[texture])
        style = self.file.createIfcSurfaceStyleWithTextures(Textures=[texture])
        ifcopenshell.api.style.remove_surface_style(self.file, style=style)
        assert len(list(self.file)) == 0
