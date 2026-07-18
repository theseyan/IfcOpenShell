# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.root
import ifcopenshell.api.unit
import pytest
from ifcopenshell.api.geometry.add_window_representation import (
    DEFAULT_PANEL_SCHEMAS,
    WindowLiningProperties,
    WindowPanelProperties,
)

import test.bootstrap


class WindowRepresentationContext:
    def body_context(self, target_view="MODEL_VIEW"):
        self.file.createIfcProject()
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        return ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Body",
            target_view=target_view,
            parent=parent,
        )


class TestAddWindowRepresentation(test.bootstrap.IFC4, WindowRepresentationContext):
    def test_creates_expected_representations_for_all_view_modes(self):
        expected = {
            "ELEVATION_VIEW": ("Curve", 1, ["IfcIndexedPolyCurve"]),
            "PLAN_VIEW": ("Curve", 8, ["IfcIndexedPolyCurve"] * 8),
            "MODEL_VIEW": (
                "SweptSolid",
                4,
                [
                    "IfcExtrudedAreaSolid",
                    "IfcExtrudedAreaSolid",
                    "IfcExtrudedAreaSolid",
                    "IfcExtrudedAreaSolid",
                ],
            ),
        }

        for target_view, (
            representation_type,
            item_count,
            item_types,
        ) in expected.items():
            context = self.body_context(target_view)
            representation = ifcopenshell.api.geometry.add_window_representation(
                self.file, context=context
            )

            assert representation.ContextOfItems == context
            assert representation.RepresentationIdentifier == "Body"
            assert representation.RepresentationType == representation_type
            assert len(representation.Items) == item_count
            assert [item.is_a() for item in representation.Items] == item_types

    def test_creates_model_representation_for_every_partition_type(self):
        expected_item_counts = {
            "SINGLE_PANEL": 4,
            "DOUBLE_PANEL_HORIZONTAL": 8,
            "DOUBLE_PANEL_VERTICAL": 8,
            "TRIPLE_PANEL_BOTTOM": 12,
            "TRIPLE_PANEL_TOP": 12,
            "TRIPLE_PANEL_LEFT": 12,
            "TRIPLE_PANEL_RIGHT": 12,
            "TRIPLE_PANEL_HORIZONTAL": 13,
            "TRIPLE_PANEL_VERTICAL": 13,
        }

        for partition_type in DEFAULT_PANEL_SCHEMAS:
            context = self.body_context()
            representation = ifcopenshell.api.geometry.add_window_representation(
                self.file,
                context=context,
                partition_type=partition_type,
                panel_properties=[{}, {}, {}],
            )

            assert representation.RepresentationType == "SweptSolid"
            assert len(representation.Items) == expected_item_counts[partition_type]

    @pytest.mark.parametrize(
        ("prefix", "width", "height"), [(None, 0.6, 0.9), ("MILLI", 600.0, 900.0)]
    )
    def test_omitted_dimensions_and_properties_follow_file_units(
        self, prefix, width, height
    ):
        self.file.createIfcProject()
        unit = ifcopenshell.api.unit.add_si_unit(
            self.file, unit_type="LENGTHUNIT", prefix=prefix
        )
        ifcopenshell.api.unit.assign_unit(self.file, [unit])
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        context = ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Body",
            target_view="ELEVATION_VIEW",
            parent=parent,
        )

        representation = ifcopenshell.api.geometry.add_window_representation(
            self.file, context=context
        )

        points = representation.Items[0].Points.CoordList
        assert max(point[0] for point in points) == pytest.approx(width)
        assert max(point[2] for point in points) == pytest.approx(height)

    def test_adds_lining_framing_and_glazing_shape_aspects(self):
        context = self.body_context()
        product = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWindow")
        product.Representation = self.file.createIfcProductDefinitionShape()

        representation = ifcopenshell.api.geometry.add_window_representation(
            self.file, context=context, part_of_product=product.Representation
        )

        aspects = {
            aspect.Name: aspect for aspect in product.Representation.HasShapeAspects
        }
        assert set(aspects) == {"Lining", "Framing", "Glazing"}
        assert len(aspects["Lining"].ShapeRepresentations[0].Items) == 2
        assert len(aspects["Framing"].ShapeRepresentations[0].Items) == 1
        assert len(aspects["Glazing"].ShapeRepresentations[0].Items) == 1
        assert all(
            aspect.ShapeRepresentations[0].RepresentationType
            == representation.RepresentationType
            for aspect in aspects.values()
        )

    def test_mutates_panel_property_inputs_to_initialised_dictionaries(self):
        context = self.body_context()
        panel_properties = [
            WindowPanelProperties(FrameDepth=0.04),
            {"FrameThickness": 0.02},
        ]

        ifcopenshell.api.geometry.add_window_representation(
            self.file,
            context=context,
            partition_type="DOUBLE_PANEL_VERTICAL",
            panel_properties=panel_properties,
        )

        assert panel_properties == [
            {
                "FrameDepth": 0.04,
                "FrameThickness": 0.035,
                "PanelPosition": None,
                "PanelOperation": None,
                "ShapeAspectStyle": None,
            },
            {
                "FrameDepth": 0.035,
                "FrameThickness": 0.02,
                "PanelPosition": None,
                "PanelOperation": None,
                "ShapeAspectStyle": None,
            },
        ]

    def test_rejects_invalid_partition_type_like_upstream(self):
        context = self.body_context()

        with pytest.raises(KeyError):
            ifcopenshell.api.geometry.add_window_representation(
                self.file, context=context, partition_type="INVALID"
            )

    def test_rejects_missing_panel_properties_like_upstream(self):
        context = self.body_context()

        with pytest.raises(IndexError):
            ifcopenshell.api.geometry.add_window_representation(
                self.file, context=context, partition_type="DOUBLE_PANEL_VERTICAL"
            )

    @pytest.mark.parametrize(
        ("arguments", "message"),
        [
            ({"partition_type": "INVALID"}, "INVALID"),
            (
                {"lining_properties": WindowLiningProperties(LiningDepth=float("nan"))},
                "must be finite",
            ),
            ({"panel_properties": []}, "list index out of range"),
        ],
    )
    def test_invalid_semantic_inputs_do_not_mutate_the_file(self, arguments, message):
        context = self.body_context()
        entity_count = len(list(self.file))

        with pytest.raises((IndexError, KeyError, ValueError), match=message):
            ifcopenshell.api.geometry.add_window_representation(
                self.file, context=context, **arguments
            )

        assert len(list(self.file)) == entity_count
