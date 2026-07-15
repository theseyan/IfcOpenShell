# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.alignment
import ifcopenshell.api.context
import ifcopenshell.api.unit
import pytest


def setup_file(schema="IFC4X3_ADD2"):
    file = ifcopenshell.file(schema=schema)
    file.createIfcProject(GlobalId=ifcopenshell.guid.new(), Name="Project")
    length = ifcopenshell.api.unit.add_si_unit(file, unit_type="LENGTHUNIT")
    ifcopenshell.api.unit.assign_unit(file, units=[length])
    model = ifcopenshell.api.context.add_context(file, context_type="Model")
    ifcopenshell.api.context.add_context(
        file,
        context_type="Model",
        context_identifier="Axis",
        target_view="MODEL_VIEW",
        parent=model,
    )
    return file


@pytest.mark.parametrize("schema", ["IFC2X3", "IFC4"])
def test_rejects_unsupported_schema_before_mutation(schema):
    file = setup_file(schema)
    before = len(list(file))
    with pytest.raises(RuntimeError, match="IFC4X3"):
        ifcopenshell.api.alignment.create(file, "Unsupported")
    assert len(list(file)) == before
    assert not file.by_type("IfcRelNests")


def test_rejects_foreign_design_parameters_without_partial_segment():
    file = setup_file()
    alignment = ifcopenshell.api.alignment.create(file, "Alignment")
    layout = ifcopenshell.api.alignment.get_horizontal_layout(alignment)
    foreign = setup_file().createIfcAlignmentHorizontalSegment(
        StartPoint=None,
        StartDirection=0.0,
        StartRadiusOfCurvature=0.0,
        EndRadiusOfCurvature=0.0,
        SegmentLength=10.0,
        PredefinedType="LINE",
    )
    before = len(file.by_type("IfcAlignmentSegment"))
    with pytest.raises(RuntimeError, match="design_parameters"):
        ifcopenshell.api.alignment.create_layout_segment(file, layout, foreign)
    assert len(file.by_type("IfcAlignmentSegment")) == before


def test_csv_path_adapter_calls_semantic_native_text_contract(tmp_path):
    file = setup_file()
    csv = tmp_path / "alignment.csv"
    csv.write_text(
        "0,0,0,100,0,25,200,100,0\n0,10,0,100,12,30,200,10,0\n", encoding="utf-8"
    )
    alignment = ifcopenshell.api.alignment.create_from_csv(file, csv)
    assert alignment.Name == "Alignment_from_CSV"
    assert ifcopenshell.api.alignment.get_horizontal_layout(alignment)
    assert len(ifcopenshell.api.alignment.get_child_alignments(alignment)) == 0
    assert ifcopenshell.api.alignment.get_vertical_layout(alignment)
    assert (
        len(
            ifcopenshell.api.alignment.get_layout_segments(
                ifcopenshell.api.alignment.get_horizontal_layout(alignment)
            )
        )
        > 1
    )


def test_zero_segment_is_idempotent_and_stays_last_after_append():
    file = setup_file()
    alignment = ifcopenshell.api.alignment.create(file, "Alignment")
    layout = ifcopenshell.api.alignment.get_horizontal_layout(alignment)
    assert ifcopenshell.api.alignment.has_zero_length_segment(layout)
    assert not ifcopenshell.api.alignment.add_zero_length_segment(file, layout)
    parameters = file.createIfcAlignmentHorizontalSegment(
        StartPoint=file.createIfcCartesianPoint((0.0, 0.0)),
        StartDirection=0.0,
        StartRadiusOfCurvature=0.0,
        EndRadiusOfCurvature=0.0,
        SegmentLength=25.0,
        PredefinedType="LINE",
    )
    segment = ifcopenshell.api.alignment.create_layout_segment(file, layout, parameters)
    assert segment.shape == (4, 4)
    semantic = ifcopenshell.api.alignment.get_layout_segments(layout)
    mapped = ifcopenshell.api.alignment.get_layout_curve(layout).Segments
    assert semantic[-1].DesignParameters.SegmentLength == 0.0
    assert mapped[-1].SegmentLength.wrappedValue == 0.0
    assert semantic[-2].DesignParameters.SegmentLength == 25.0


def test_vertical_clothoid_remains_explicitly_unsupported():
    file = setup_file()
    segment = file.createIfcAlignmentSegment(
        GlobalId=ifcopenshell.guid.new(),
        DesignParameters=file.createIfcAlignmentVerticalSegment(
            StartDistAlong=0.0,
            HorizontalLength=10.0,
            StartHeight=0.0,
            StartGradient=0.0,
            EndGradient=0.0,
            PredefinedType="CLOTHOID",
        ),
    )
    from ifcopenshell.api.alignment._map_alignment_vertical_segment import (
        _map_alignment_vertical_segment,
    )

    with pytest.raises(RuntimeError, match="CLOTHOID"):
        _map_alignment_vertical_segment(file, segment)
