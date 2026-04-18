
import ifcopenshell.api.material
import ifcopenshell.api.style
import test.bootstrap


class TestRemoveStyle(test.bootstrap.IFC4):
    def test_removing_a_style(self):
        shading = self.file.createIfcSurfaceStyleShading()
        style = self.file.createIfcSurfaceStyle(Styles=[shading])
        ifcopenshell.api.style.remove_style(self.file, style=style)
        assert len(list(self.file)) == 0

    def test_removing_any_styled_items_referencing_the_style(self):
        shading = self.file.createIfcSurfaceStyleShading()
        style = self.file.createIfcSurfaceStyle(Styles=[shading])
        styled_item = self.file.createIfcStyledItem(Styles=[style])
        ifcopenshell.api.style.remove_style(self.file, style=style)
        assert len(list(self.file)) == 0

    def test_remove_non_surface_styles(self):
        STYLE_TYPES = [
            "IfcCurveStyle",
            "IfcFillAreaStyle",
            "IfcTextStyle",
        ]
        for style_type in STYLE_TYPES:
            style = self.file.create_entity(style_type)
            if style_type == "IfcFillAreaStyle":
                style.FillStyles = (self.file.create_entity("IfcDraughtingPreDefinedColour", Name="cyan"),)
            ifcopenshell.api.style.remove_style(self.file, style=style)
            assert len(self.file.by_type("IfcPresentationStyle")) == 0


class TestRemoveStyleIFC2X3(test.bootstrap.IFC2X3, TestRemoveStyle):
    pass
