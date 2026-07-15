# SPDX-License-Identifier: LGPL-3.0-or-later

import inspect

import ifcopenshell
import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.root
import ifcopenshell.api.unit
import numpy as np
import pytest
from ifcopenshell.api.geometry import compute_wall_mounted_handrail_geometry as compute

COMMON = dict(
    support_spacing=1.0,
    railing_diameter=0.05,
    clear_width=0.04,
    height=1.0,
    use_manual_supports=False,
    looped_path=False,
    unit_scale=1.0,
)


@pytest.mark.parametrize(
    ("terminal", "indices", "points"),
    [
        (
            "180",
            [1, 4],
            [
                [0, 0, 0.725],
                [-0.15, 0, 0.875],
                [0, 0, 1.025],
                [2, 0, 1.025],
                [2.15, 0, 0.875],
                [2, 0, 0.725],
            ],
        ),
        (
            "TO_END_POST",
            [2, 5],
            [
                [2, 0, 0.725],
                [0, 0, 0.725],
                [-0.15, 0, 0.875],
                [0, 0, 1.025],
                [2, 0, 1.025],
                [2.15, 0, 0.875],
                [2, 0, 0.725],
                [0, 0, 0.725],
            ],
        ),
        (
            "TO_WALL",
            [1, 4],
            [
                [-0.04, -0.04, 1.025],
                [-0.028284271247461905, -0.011715728752538098, 1.025],
                [0, 0, 1.025],
                [2, 0, 1.025],
                [2.028284271247462, -0.011715728752538098, 1.025],
                [2.04, -0.04, 1.025],
            ],
        ),
        (
            "TO_FLOOR",
            [2, 5],
            [
                [-0.15, 0, 0.05],
                [-0.15, 0, 0.875],
                [-0.10606601717798213, 0, 0.981066017177982],
                [0, 0, 1.025],
                [2, 0, 1.025],
                [2.106066017177982, 0, 0.981066017177982],
                [2.15, 0, 0.875],
                [2.15, 0, 0.05],
            ],
        ),
        ("NONE", [], [[0, 0, 1.025], [2, 0, 1.025]]),
    ],
)
def test_terminal_coordinates_and_arc_order(terminal, indices, points):
    result = compute(
        railing_path=[(0, 0, 1), (2, 0, 1)], terminal_type=terminal, **COMMON
    )
    assert result.handrail_arc_point_indices == indices
    np.testing.assert_allclose(result.handrail_polyline, points)


def test_combined_terminal_has_two_ordered_fillets_per_end():
    result = compute(
        railing_path=[(0, 0, 1), (2, 0, 1)],
        terminal_type="TO_END_POST_AND_FLOOR",
        **COMMON,
    )
    assert result.handrail_arc_point_indices == [2, 5, 12, 15]
    np.testing.assert_allclose(
        result.handrail_polyline,
        [
            [2, 0, 0.05],
            [-0.04493071589424352, 0, 0.6920131317342391],
            [-0.12091238022662631, 0, 0.7463553125636206],
            [-0.15, 0, 0.8351257823603483],
            [-0.15, 0, 0.875],
            [-0.10606601717798206, 0, 0.981066017177982],
            [0, 0, 1.025],
            [0, 0, 1.025],
            [0, 0, 1.025],
            [2, 0, 1.025],
            [2, 0, 1.025],
            [2, 0, 1.025],
            [2.1060660171779824, 0, 0.9810660171779819],
            [2.15, 0, 0.875],
            [2.15, 0, 0.8351257823603483],
            [2.120912380226626, 0, 0.7463553125636205],
            [2.0449307158942434, 0, 0.6920131317342391],
            [0, 0, 0.05],
        ],
        atol=1e-14,
    )


def test_manual_collinear_vertex_and_unused_spacing_policy():
    result = compute(
        railing_path=[(0, 0, 1), (1, 0, 1), (2, 0, 1)],
        terminal_type="NONE",
        use_manual_supports=True,
        support_spacing=0.0,
        **{
            k: v
            for k, v in COMMON.items()
            if k not in {"use_manual_supports", "support_spacing"}
        },
    )
    assert len(result.supports) == 1
    np.testing.assert_allclose(result.supports[0].arc_polyline[0], [1, 0, 1.025])


@pytest.mark.parametrize(
    ("spacing", "expected_x"),
    [(1.0, [0.0, 1.0, 2.0]), (0.75, [0.25, 1.0, 1.75])],
)
def test_automatic_spacing_exact_multiples_and_remainders(spacing, expected_x):
    result = compute(
        railing_path=[(0, 0, 1), (2, 0, 1)],
        terminal_type="NONE",
        support_spacing=spacing,
        **{k: v for k, v in COMMON.items() if k != "support_spacing"},
    )
    assert [support.arc_polyline[0, 0] for support in result.supports] == pytest.approx(
        expected_x
    )


def test_support_order_is_run_then_position():
    result = compute(
        railing_path=[(0, 0, 1), (2, 0, 1), (2, 2, 1)], terminal_type="NONE", **COMMON
    )
    starts = [support.arc_polyline[0].tolist() for support in result.supports]
    np.testing.assert_allclose(
        starts,
        [
            [0, 0, 1.025],
            [1, 0, 1.025],
            [2, 0, 1.025],
            [2, 0, 1.025],
            [2, 1, 1.025],
            [2, 2, 1.025],
        ],
    )


@pytest.mark.parametrize(
    ("path", "looped"),
    [
        ([(0, 0, 1), (0, 0, 1), (1, 0, 1)], False),
        ([(0, 0, 1), (1, 0, 1), (1, 0, 1), (2, 0, 1)], False),
        ([(0, 0, 1), (1, 0, 1), (1, 0, 1)], False),
        ([(0, 0, 1), (0, 0, 1), (0, 0, 1)], False),
        ([(0, 0, 1), (1, 0, 1), (0, 0, 1)], False),
        ([(0, 0, 1), (0, 0, 2), (0, 0, 3)], False),
        ([(0, 0, 1), (1, 0, 1), (1, 1, 1), (0, 1, 1), (0, 0, 1)], True),
    ],
)
def test_degenerate_policies_are_finite(path, looped):
    result = compute(
        railing_path=path,
        terminal_type="NONE",
        looped_path=looped,
        **{k: v for k, v in COMMON.items() if k != "looped_path"},
    )
    assert np.isfinite(result.handrail_polyline).all()
    assert all(np.isfinite(support.arc_polyline).all() for support in result.supports)


@pytest.mark.parametrize(
    "overrides",
    [
        {"railing_path": [(0, 0, 1)]},
        {"railing_path": [(0, 0, 1), (float("nan"), 0, 1)]},
        {"terminal_type": "INVALID"},
        {"railing_diameter": 0.0},
        {"clear_width": 0.0},
        {"unit_scale": 0.0},
        {"height": 0.01},
        {"support_spacing": 0.0},
    ],
)
def test_invalid_inputs_raise_clear_errors(overrides):
    kwargs = dict(COMMON, railing_path=[(0, 0, 1), (2, 0, 1)], terminal_type="NONE")
    kwargs.update(overrides)
    with pytest.raises((RuntimeError, ValueError)):
        compute(**kwargs)


def _context(schema="IFC4", millimetres=False):
    file = ifcopenshell.file(schema=schema)
    file.create_entity("IfcProject")
    if millimetres:
        unit = ifcopenshell.api.unit.add_si_unit(
            file, unit_type="LENGTHUNIT", prefix="MILLI"
        )
        ifcopenshell.api.unit.assign_unit(file, [unit])
    parent = ifcopenshell.api.context.add_context(file, context_type="Model")
    context = ifcopenshell.api.context.add_context(
        file,
        context_type="Model",
        context_identifier="Body",
        target_view="MODEL_VIEW",
        parent=parent,
    )
    return file, context


@pytest.mark.parametrize("schema", ["IFC4", "IFC4X3"])
def test_ifc_materialization_and_final_handrail(schema):
    file, context = _context(schema)
    representation = ifcopenshell.api.geometry.add_railing_representation(
        file, context=context, railing_path=[(0, 0, 1), (2, 0, 1)], terminal_type="NONE"
    )
    assert representation.ContextOfItems == context
    assert representation.RepresentationType == "SolidModel"
    assert len(representation.Items) == 7
    assert representation.Items[-1].is_a("IfcSweptDiskSolid")


def test_ifc2x3_arc_limitation_is_rejected_before_mutation():
    file, context = _context("IFC2X3")
    before = len(file)
    with pytest.raises(RuntimeError, match="not supported for IFC2X3"):
        ifcopenshell.api.geometry.add_railing_representation(
            file,
            context=context,
            railing_path=[(0, 0, 1), (2, 0, 1)],
            terminal_type="NONE",
        )
    assert len(file) == before


def test_wrapper_defaults_follow_millimetre_project_units():
    file, context = _context(millimetres=True)
    representation = ifcopenshell.api.geometry.add_railing_representation(
        file,
        context=context,
        railing_path=[(0, 0, 1000), (2000, 0, 1000)],
        terminal_type="NONE",
    )
    assert len(representation.Items) == 7
    assert representation.Items[-1].Radius == pytest.approx(25.0)


def test_none_path_deliberately_uses_upstream_compatibility_default():
    file, context = _context()
    representation = ifcopenshell.api.geometry.add_railing_representation(
        file, context=context, railing_path=None, terminal_type="NONE"
    )
    assert len(representation.Items) == 7
    direct = compute(
        railing_path=[(0, 0, 1), (1, 0, 1), (2, 0, 1)], terminal_type="NONE", **COMMON
    )
    assert representation.Items[-1].Radius == pytest.approx(direct.handrail_radius)


def test_context_ownership_and_invalid_compute_are_atomic():
    file, context = _context()
    other, other_context = _context()
    before = len(file)
    with pytest.raises(RuntimeError, match="context"):
        ifcopenshell.api.geometry.add_railing_representation(
            file, context=other_context, railing_path=[(0, 0, 1), (2, 0, 1)]
        )
    assert len(file) == before
    with pytest.raises(RuntimeError):
        ifcopenshell.api.geometry.add_railing_representation(
            file, context=context, railing_path=[(0, 0, 1)], terminal_type="NONE"
        )
    assert len(file) == before
    assert other_context.file == other


def test_public_exports_and_signature_removed_railing_type():
    expected = {
        "TERMINAL_TYPE",
        "RailingSupport",
        "WallMountedHandrailGeometry",
        "compute_wall_mounted_handrail_geometry",
        "add_railing_representation",
    }
    module = ifcopenshell.api.geometry
    assert expected <= set(module.__all__)
    assert (
        "railing_type"
        not in inspect.signature(module.add_railing_representation).parameters
    )
