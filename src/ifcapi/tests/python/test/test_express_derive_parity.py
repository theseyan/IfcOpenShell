# Cross-schema parity coverage for the native EXPRESS DERIVE rule
# evaluator. Each test instantiates a small in-memory entity tree and
# compares the value returned through __getattr__ (which routes through
# `ifcopenshell_compute_derived` in the C ABI) against a hand-computed
# expected result drawn from the IFC EXPRESS specification.

import os
import sys

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "python"))
import ifcopenshell


SCHEMAS = ["IFC2X3", "IFC4", "IFC4X1", "IFC4X3_ADD2"]


@pytest.fixture(params=SCHEMAS)
def f(request):
    return ifcopenshell.file(schema=request.param)


def _pt(f, *coords):
    return f.create_entity("IfcCartesianPoint", Coordinates=tuple(float(c) for c in coords))


def _dir(f, *ratios):
    return f.create_entity("IfcDirection", DirectionRatios=tuple(float(r) for r in ratios))


class TestDimDerivedAttribute:
    """`Dim` is the most widely-used derived attribute in IFC and is
    overridden across the full geometric type hierarchy."""

    def test_cartesian_point_2d(self, f):
        assert _pt(f, 1.0, 2.0).Dim == 2

    def test_cartesian_point_3d(self, f):
        assert _pt(f, 1.0, 2.0, 3.0).Dim == 3

    def test_direction_2d(self, f):
        assert _dir(f, 1.0, 0.0).Dim == 2

    def test_direction_3d(self, f):
        assert _dir(f, 0.0, 0.0, 1.0).Dim == 3

    def test_vector(self, f):
        v = f.create_entity("IfcVector", Orientation=_dir(f, 1.0, 0.0, 0.0), Magnitude=2.5)
        assert v.Dim == 3

    def test_line(self, f):
        v = f.create_entity("IfcVector", Orientation=_dir(f, 1.0, 0.0, 0.0), Magnitude=1.0)
        ln = f.create_entity("IfcLine", Pnt=_pt(f, 0.0, 0.0, 0.0), Dir=v)
        assert ln.Dim == 3

    def test_polyline(self, f):
        poly = f.create_entity("IfcPolyline", Points=[_pt(f, 0.0, 0.0, 0.0), _pt(f, 1.0, 1.0, 1.0)])
        assert poly.Dim == 3

    def test_axis2placement2d(self, f):
        a2 = f.create_entity(
            "IfcAxis2Placement2D",
            Location=_pt(f, 0.0, 0.0),
            RefDirection=_dir(f, 1.0, 0.0),
        )
        assert a2.Dim == 2

    def test_axis2placement3d(self, f):
        a3 = f.create_entity(
            "IfcAxis2Placement3D",
            Location=_pt(f, 0.0, 0.0, 0.0),
            Axis=_dir(f, 0.0, 0.0, 1.0),
            RefDirection=_dir(f, 1.0, 0.0, 0.0),
        )
        assert a3.Dim == 3

    def test_circle(self, f):
        a2 = f.create_entity("IfcAxis2Placement2D", Location=_pt(f, 0.0, 0.0))
        ci = f.create_entity("IfcCircle", Position=a2, Radius=1.0)
        assert ci.Dim == 2

    def test_bounding_box(self, f):
        bb = f.create_entity(
            "IfcBoundingBox",
            Corner=_pt(f, 0.0, 0.0, 0.0),
            XDim=1.0,
            YDim=2.0,
            ZDim=3.0,
        )
        assert bb.Dim == 3


class TestBSplineDerivedAttributes:
    """B-spline derived indices exercise SIZEOF over LIST attributes."""

    def _bspline(self, f, n_cps, degree):
        cps = [_pt(f, float(i), 0.0, 0.0) for i in range(n_cps)]
        return f.create_entity(
            "IfcBSplineCurveWithKnots",
            Degree=degree,
            ControlPointsList=cps,
            CurveForm="UNSPECIFIED",
            ClosedCurve=False,
            SelfIntersect=False,
            KnotMultiplicities=[degree + 1, degree + 1],
            Knots=[0.0, 1.0],
            KnotSpec="UNSPECIFIED",
        )

    def test_upper_index_on_control_points(self, f):
        # IFC2X3 doesn't have IfcBSplineCurveWithKnots
        if f.schema == "IFC2X3":
            pytest.skip("IfcBSplineCurveWithKnots is IFC4+")
        bsc = self._bspline(f, n_cps=4, degree=3)
        assert bsc.UpperIndexOnControlPoints == 3

    def test_upper_index_on_knots(self, f):
        if f.schema == "IFC2X3":
            pytest.skip("IfcBSplineCurveWithKnots is IFC4+")
        bsc = self._bspline(f, n_cps=4, degree=3)
        assert bsc.UpperIndexOnKnots == 2


class TestEntityReturningDerives:
    """DERIVE rules that synthesize new entities must cross the ABI as usable
    ``entity_instance`` wrappers backed by the scratch file."""

    def test_axis2placement3d_p_returns_entities(self, f):
        a3 = f.create_entity(
            "IfcAxis2Placement3D",
            Location=_pt(f, 0.0, 0.0, 0.0),
            Axis=_dir(f, 0.0, 0.0, 1.0),
            RefDirection=_dir(f, 1.0, 0.0, 0.0),
        )
        p = a3.P
        assert len(p) == 3
        expected = (
            pytest.approx((1.0, 0.0, 0.0)),
            pytest.approx((0.0, 1.0, 0.0)),
            pytest.approx((0.0, 0.0, 1.0)),
        )
        for axis, ratios in zip(p, expected):
            assert axis.is_a("IfcDirection")
            assert axis.DirectionRatios == ratios
            rebound = axis.file.by_id(axis.id())
            assert rebound.is_a("IfcDirection")
            assert rebound.DirectionRatios == ratios
