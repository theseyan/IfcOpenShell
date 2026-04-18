
import ifcopenshell.api.boundary
import ifcopenshell.api.root
import test.bootstrap


class TestEditAttributes(test.bootstrap.IFC4):
    def setup_boundary(self):
        space = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSpace")
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")
        return boundary, space, wall

    def test_sets_relating_space_and_building_element(self):
        boundary, space, wall = self.setup_boundary()
        ifcopenshell.api.boundary.edit_attributes(
            self.file, entity=boundary, relating_space=space, related_building_element=wall
        )
        assert boundary.RelatingSpace == space
        assert boundary.RelatedBuildingElement == wall

    def test_defaults_enums_to_notdefined(self):
        boundary, space, wall = self.setup_boundary()
        ifcopenshell.api.boundary.edit_attributes(
            self.file, entity=boundary, relating_space=space, related_building_element=wall
        )
        assert boundary.PhysicalOrVirtualBoundary == "NOTDEFINED"
        assert boundary.InternalOrExternalBoundary == "NOTDEFINED"

    def test_sets_physical_or_virtual(self):
        boundary, space, wall = self.setup_boundary()
        ifcopenshell.api.boundary.edit_attributes(
            self.file, entity=boundary, relating_space=space, related_building_element=wall,
            physical_or_virtual="PHYSICAL",
        )
        assert boundary.PhysicalOrVirtualBoundary == "PHYSICAL"

    def test_sets_internal_or_external(self):
        boundary, space, wall = self.setup_boundary()
        ifcopenshell.api.boundary.edit_attributes(
            self.file, entity=boundary, relating_space=space, related_building_element=wall,
            internal_or_external="EXTERNAL",
        )
        assert boundary.InternalOrExternalBoundary == "EXTERNAL"

    def test_sets_all_enum_variants(self):
        boundary, space, wall = self.setup_boundary()
        for value in ("PHYSICAL", "VIRTUAL", "NOTDEFINED"):
            ifcopenshell.api.boundary.edit_attributes(
                self.file, entity=boundary, relating_space=space, related_building_element=wall,
                physical_or_virtual=value,
            )
            assert boundary.PhysicalOrVirtualBoundary == value

        for value in ("INTERNAL", "EXTERNAL", "EXTERNAL_EARTH", "EXTERNAL_WATER", "EXTERNAL_FIRE", "NOTDEFINED"):
            ifcopenshell.api.boundary.edit_attributes(
                self.file, entity=boundary, relating_space=space, related_building_element=wall,
                internal_or_external=value,
            )
            assert boundary.InternalOrExternalBoundary == value


class TestEditAttributesIFC2X3(test.bootstrap.IFC2X3, TestEditAttributes):
    def test_sets_all_enum_variants(self):
        boundary, space, wall = self.setup_boundary()
        for value in ("PHYSICAL", "VIRTUAL", "NOTDEFINED"):
            ifcopenshell.api.boundary.edit_attributes(
                self.file, entity=boundary, relating_space=space, related_building_element=wall,
                physical_or_virtual=value,
            )
            assert boundary.PhysicalOrVirtualBoundary == value

        # IFC2X3 only has INTERNAL, EXTERNAL, NOTDEFINED
        for value in ("INTERNAL", "EXTERNAL", "NOTDEFINED"):
            ifcopenshell.api.boundary.edit_attributes(
                self.file, entity=boundary, relating_space=space, related_building_element=wall,
                internal_or_external=value,
            )
            assert boundary.InternalOrExternalBoundary == value
