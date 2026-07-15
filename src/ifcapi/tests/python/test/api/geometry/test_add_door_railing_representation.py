# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import pytest

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.root
from ifcopenshell.api.geometry.add_door_representation import DoorLiningProperties, SUPPORTED_DOOR_TYPES
import test.bootstrap


class GeometryConstructorContext:
    def context(self, target_view="MODEL_VIEW", context_identifier="Body"):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        return ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier=context_identifier,
            target_view=target_view,
            parent=parent,
        )


class TestAddDoorRepresentation(test.bootstrap.IFC4, GeometryConstructorContext):
    def test_creates_expected_representations_for_view_modes(self):
        expected = {
            "ELEVATION_VIEW": ("Curve3D", 1),
            "PLAN_VIEW": ("Curve2D", 4),
            "MODEL_VIEW": ("SweptSolid", 8),
        }

        for target_view, (representation_type, item_count) in expected.items():
            context = self.context(target_view)
            representation = ifcopenshell.api.geometry.add_door_representation(self.file, context=context)

            assert representation.ContextOfItems == context
            assert representation.RepresentationIdentifier == "Body"
            assert representation.RepresentationType == representation_type
            assert len(representation.Items) == item_count

    def test_creates_model_representation_for_every_supported_door_type(self):
        context = self.context()

        for operation_type in SUPPORTED_DOOR_TYPES:
            representation = ifcopenshell.api.geometry.add_door_representation(
                self.file, context=context, operation_type=operation_type
            )
            assert representation.RepresentationType == "SweptSolid"
            assert len(representation.Items) > 0

    def test_non_sliding_annotation_plan_returns_none(self):
        context = self.context("PLAN_VIEW", "Annotation")

        assert ifcopenshell.api.geometry.add_door_representation(self.file, context=context) is None

    def test_sliding_annotation_plan_creates_curve2d(self):
        context = self.context("PLAN_VIEW", "Annotation")

        representation = ifcopenshell.api.geometry.add_door_representation(
            self.file, context=context, operation_type="SLIDING_TO_LEFT"
        )

        assert representation.RepresentationType == "Curve2D"
        assert len(representation.Items) == 2

    def test_adds_shape_aspects_with_transom_glazing(self):
        context = self.context()
        product = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcDoor")
        product.Representation = self.file.createIfcProductDefinitionShape()

        ifcopenshell.api.geometry.add_door_representation(
            self.file,
            context=context,
            lining_properties=DoorLiningProperties(TransomThickness=0.05),
            part_of_product=product.Representation,
        )

        aspects = {aspect.Name: aspect for aspect in product.Representation.HasShapeAspects}
        assert set(aspects) == {"Lining", "Framing", "Glazing"}
        assert len(aspects["Glazing"].ShapeRepresentations[0].Items) == 1


class TestAddRailingRepresentation(test.bootstrap.IFC4, GeometryConstructorContext):
    def test_creates_default_wall_mounted_handrail(self):
        context = self.context()

        representation = ifcopenshell.api.geometry.add_railing_representation(
            self.file, context=context, railing_path=[(0.0, 0.0, 1.0), (1.0, 0.0, 1.0), (2.0, 0.0, 1.0)]
        )

        assert representation.ContextOfItems == context
        assert representation.RepresentationType == "SolidModel"
        assert len(representation.Items) == 7

    def test_creates_representations_for_terminal_variants(self):
        context = self.context()
        path = [(0.0, 0.0, 1.0), (1.0, 0.0, 1.0), (1.0, 1.0, 1.0)]

        for terminal_type in ("180", "TO_END_POST", "TO_WALL", "TO_FLOOR", "TO_END_POST_AND_FLOOR"):
            representation = ifcopenshell.api.geometry.add_railing_representation(
                self.file, context=context, railing_path=path, terminal_type=terminal_type
            )
            assert representation.RepresentationType == "SolidModel"
            assert len(representation.Items) > 0

    def test_manual_supports_and_looped_path(self):
        context = self.context()

        representation = ifcopenshell.api.geometry.add_railing_representation(
            self.file,
            context=context,
            railing_path=[(0.0, 0.0, 1.0), (1.0, 0.0, 1.0), (1.0, 1.0, 1.0)],
            use_manual_supports=True,
            looped_path=True,
        )

        assert representation.RepresentationType == "SolidModel"
        assert len(representation.Items) > 0
