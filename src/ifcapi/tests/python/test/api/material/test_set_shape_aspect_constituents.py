# IfcOpenShell - IFC toolkit and geometry engine
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.material
import ifcopenshell.api.root
import ifcopenshell.api.style
import ifcopenshell.api.type
import ifcopenshell.util.element
import ifcopenshell.util.shape_builder

import test.bootstrap


class TestSetShapeAspectConstituents(test.bootstrap.IFC4):
    def make_contexts(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        model = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        body = ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Body",
            target_view="MODEL_VIEW",
            parent=model,
        )
        other = ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Box",
            target_view="MODEL_VIEW",
            parent=model,
        )
        return body, other

    def make_geometry(self, element=None, names=("Frame", "Glass"), context=None):
        body = context or self.make_contexts()[0]
        builder = ifcopenshell.util.shape_builder.ShapeBuilder(self.file)
        items = [builder.sphere() for _ in names]
        representation = builder.get_representation(body, items)
        element = element or ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcWall"
        )
        ifcopenshell.api.geometry.assign_representation(
            self.file, product=element, representation=representation
        )
        part = (
            element.Representation
            if element.is_a("IfcProduct")
            else element.RepresentationMaps[0]
        )
        for name, item in zip(names, items):
            if name:
                ifcopenshell.api.geometry.add_shape_aspect(
                    self.file,
                    name=name,
                    items=[item],
                    representation=representation,
                    part_of_product=part,
                )
        return element, body, representation, items

    def make_styled_material(self, context, name):
        material = ifcopenshell.api.material.add_material(self.file, name=name)
        style = ifcopenshell.api.style.add_style(self.file, name=f"{name} style")
        ifcopenshell.api.style.assign_material_style(
            self.file, material=material, style=style, context=context
        )
        return material, style

    def current_material(self, element):
        return ifcopenshell.util.element.get_material(element, should_inherit=False)

    def test_first_assignment_order_and_empty_mapping(self):
        element, body, _, _ = self.make_geometry(names=())
        first = ifcopenshell.api.material.add_material(self.file, name="First")
        second = ifcopenshell.api.material.add_material(self.file, name="Second")
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file,
            element=element,
            context=body,
            materials={"B": second, "A": first},
        )
        material_set = self.current_material(element)
        assert [item.Name for item in material_set.MaterialConstituents] == ["B", "A"]
        assert [item.Material for item in material_set.MaterialConstituents] == [
            second,
            first,
        ]

        empty_element = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcWall"
        )
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=empty_element, context=body, materials={}
        )
        assert self.current_material(empty_element).MaterialConstituents == ()

    def test_exact_mapping_is_reused_even_when_order_changes(self):
        element, body, _, _ = self.make_geometry(names=())
        first = ifcopenshell.api.material.add_material(self.file, name="First")
        second = ifcopenshell.api.material.add_material(self.file, name="Second")
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file,
            element=element,
            context=body,
            materials={"A": first, "B": second},
        )
        material_set = self.current_material(element)
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file,
            element=element,
            context=body,
            materials={"B": second, "A": first},
        )
        assert self.current_material(element) == material_set
        assert len(self.file.by_type("IfcMaterialConstituentSet")) == 1

    def test_same_names_with_different_materials_replace_the_assignment(self):
        element, body, _, _ = self.make_geometry(names=())
        first = ifcopenshell.api.material.add_material(self.file, name="First")
        second = ifcopenshell.api.material.add_material(self.file, name="Second")
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=element, context=body, materials={"A": first}
        )
        old_set = self.current_material(element)
        old_item = old_set.MaterialConstituents[0]
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=element, context=body, materials={"A": second}
        )
        assert self.current_material(element) != old_set
        assert self.current_material(element).MaterialConstituents[0].Material == second
        assert old_set not in self.file.by_type("IfcMaterialConstituentSet")
        assert old_item not in self.file.by_type("IfcMaterialConstituent")

    def test_shared_set_and_bare_material_are_retained(self):
        body, _ = self.make_contexts()
        first = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        second = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        material = ifcopenshell.api.material.add_material(self.file, name="Old")
        shared = ifcopenshell.api.material.add_material_set(
            self.file, set_type="IfcMaterialConstituentSet"
        )
        ifcopenshell.api.material.add_constituent(
            self.file, constituent_set=shared, material=material, name="Old"
        )
        ifcopenshell.api.material.assign_material(
            self.file, products=[first, second], material=shared
        )
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=first, context=body, materials={"New": material}
        )
        assert shared in self.file.by_type("IfcMaterialConstituentSet")
        assert self.current_material(second) == shared

        third = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        bare = ifcopenshell.api.material.add_material(self.file, name="Bare")
        ifcopenshell.api.material.assign_material(
            self.file, products=[third], material=bare
        )
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=third, context=body, materials={"New": material}
        )
        assert bare in self.file.by_type("IfcMaterial")

    def test_inherited_material_is_retained_and_occurrence_gets_own_set(self):
        body, _ = self.make_contexts()
        wall_type = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcWallType"
        )
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(
            self.file, related_objects=[wall], relating_type=wall_type
        )
        old_material = ifcopenshell.api.material.add_material(self.file, name="Old")
        old_set = ifcopenshell.api.material.add_material_set(
            self.file, set_type="IfcMaterialConstituentSet"
        )
        ifcopenshell.api.material.add_constituent(
            self.file, constituent_set=old_set, material=old_material, name="Old"
        )
        ifcopenshell.api.material.assign_material(
            self.file, products=[wall_type], material=old_set
        )
        new_material = ifcopenshell.api.material.add_material(self.file, name="New")
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=wall, context=body, materials={"New": new_material}
        )
        assert self.current_material(wall) != old_set
        assert self.current_material(wall_type) == old_set
        assert old_set in self.file.by_type("IfcMaterialConstituentSet")

    def test_assigns_styles_by_exact_aspect_and_context_and_replaces_existing_style(
        self,
    ):
        body, other = self.make_contexts()
        element, _, _, items = self.make_geometry(
            names=("Frame", "Glass", ""), context=body
        )
        frame, frame_style = self.make_styled_material(body, "Frame")
        glass, _ = self.make_styled_material(other, "Glass")
        old_style = ifcopenshell.api.style.add_style(self.file, name="Old")
        ifcopenshell.api.style.assign_item_style(
            self.file, item=items[0], style=old_style
        )
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file,
            element=element,
            context=body,
            materials={"Frame": frame, "Glass": glass},
        )
        assert items[0].StyledByItem[0].Styles == (frame_style,)
        assert items[1].StyledByItem == ()
        assert items[2].StyledByItem == ()

    def test_type_product_and_mapped_occurrence_representations_are_styled(self):
        body, _ = self.make_contexts()
        wall_type = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcWallType"
        )
        _, _, _, type_items = self.make_geometry(
            element=wall_type, names=("Frame",), context=body
        )
        material, style = self.make_styled_material(body, "Frame")
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=wall_type, context=body, materials={"Frame": material}
        )
        assert type_items[0].StyledByItem[0].Styles == (style,)

        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.type.assign_type(
            self.file, related_objects=[wall], relating_type=wall_type
        )
        mapped = ifcopenshell.api.geometry.map_representation(
            self.file, representation=wall_type.RepresentationMaps[0]
        )
        ifcopenshell.api.geometry.assign_representation(
            self.file, product=wall, representation=mapped
        )
        ifcopenshell.api.material.set_shape_aspect_constituents(
            self.file, element=wall, context=body, materials={"Frame": material}
        )
        assert type_items[0].StyledByItem[0].Styles == (style,)

    def test_missing_representation_is_a_safe_material_only_operation(self):
        body, _ = self.make_contexts()
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        material = ifcopenshell.api.material.add_material(self.file, name="M")
        assert (
            ifcopenshell.api.material.set_shape_aspect_constituents(
                self.file, element=wall, context=body, materials={"Part": material}
            )
            is None
        )
        assert self.current_material(wall).MaterialConstituents[0].Material == material
