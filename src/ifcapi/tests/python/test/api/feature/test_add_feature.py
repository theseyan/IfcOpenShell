
import pytest

import ifcopenshell.api.feature
import ifcopenshell.api.root
import test.bootstrap


class TestAddFeature(test.bootstrap.IFC4):
    def test_adding_an_opening(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        opening = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcOpeningElement")
        ifcopenshell.api.feature.add_feature(self.file, feature=opening, element=wall)
        assert wall.HasOpenings[0].RelatedOpeningElement == opening

    def test_adding_a_projection(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        projection = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProjectionElement")
        ifcopenshell.api.feature.add_feature(self.file, feature=projection, element=wall)
        assert wall.HasProjections[0].RelatedFeatureElement == projection

    def test_adding_a_surface_feature(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        feature = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSurfaceFeature")
        ifcopenshell.api.feature.add_feature(self.file, feature=feature, element=wall)
        assert wall.IsDecomposedBy[0].RelatedObjects == (feature,)

    def test_adding_an_opening_twice(self):
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        opening = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcOpeningElement")
        ifcopenshell.api.feature.add_feature(self.file, feature=opening, element=wall)
        ifcopenshell.api.feature.add_feature(self.file, feature=opening, element=wall)
        assert wall.HasOpenings[0].RelatedOpeningElement == opening
        assert len(wall.HasOpenings) == 1

    def test_adding_an_opening_which_is_already_voiding_another_element(self):
        slab = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        opening = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcOpeningElement")
        ifcopenshell.api.feature.add_feature(self.file, feature=opening, element=slab)
        ifcopenshell.api.feature.add_feature(self.file, feature=opening, element=wall)
        assert not slab.HasOpenings
        assert wall.HasOpenings[0].RelatedOpeningElement == opening


class TestAddFeatureIFC2X3(test.bootstrap.IFC2X3, TestAddFeature):
    def test_adding_a_surface_feature(self):
        pass
