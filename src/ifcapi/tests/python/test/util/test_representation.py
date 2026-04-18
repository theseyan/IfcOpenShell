# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for the native-backed helpers in ``ifcopenshell.util.representation``.

Covers the ported C++ functions: ``get_context``, ``resolve_representation``,
and ``get_representation``. Scenarios exercise both IFC4 and IFC2X3 and the
IfcProduct vs IfcTypeProduct branches.
"""

import pytest

import ifcopenshell.util.representation as subject
import test.bootstrap


def _ctx(file_, context_type="Model", identifier=None, target_view=None, parent=None):
    if identifier or target_view:
        ctx = file_.create_entity(
            "IfcGeometricRepresentationSubContext",
            ContextType=context_type,
            ContextIdentifier=identifier,
            TargetView=target_view or "MODEL_VIEW",
            ParentContext=parent or _ctx(file_, context_type),
        )
    else:
        ctx = file_.create_entity(
            "IfcGeometricRepresentationContext",
            ContextType=context_type,
            CoordinateSpaceDimension=3,
        )
    return ctx


def _rep(file_, ctx, items=None, identifier="Body", rep_type="SweptSolid"):
    return file_.create_entity(
        "IfcShapeRepresentation",
        ContextOfItems=ctx,
        RepresentationIdentifier=identifier,
        RepresentationType=rep_type,
        Items=items or [file_.create_entity("IfcExtrudedAreaSolid")],
    )


# -----------------------------------------------------------------------------
# get_context
# -----------------------------------------------------------------------------


class TestGetContextIFC4(test.bootstrap.IFC4):
    def test_none_when_missing(self):
        assert subject.get_context(self.file, "Model") is None

    def test_matches_base_context_only(self):
        model = _ctx(self.file, "Model")
        assert subject.get_context(self.file, "Model") == model

    def test_excludes_subcontext_when_base_requested(self):
        model = _ctx(self.file, "Model")
        _ctx(self.file, "Model", identifier="Body", parent=model)
        assert subject.get_context(self.file, "Model") == model

    def test_matches_subcontext_by_identifier(self):
        model = _ctx(self.file, "Model")
        sub = _ctx(self.file, "Model", identifier="Body", parent=model)
        assert subject.get_context(self.file, "Model", "Body") == sub

    def test_subcontext_identifier_mismatch_returns_none(self):
        model = _ctx(self.file, "Model")
        _ctx(self.file, "Model", identifier="Body", parent=model)
        assert subject.get_context(self.file, "Model", "Annotation") is None

    def test_matches_by_target_view(self):
        model = _ctx(self.file, "Model")
        sub = _ctx(self.file, "Model", identifier="Body",
                   target_view="MODEL_VIEW", parent=model)
        assert subject.get_context(self.file, "Model", "Body", "MODEL_VIEW") == sub


class TestGetContextIFC2X3(test.bootstrap.IFC2X3):
    def test_matches_subcontext(self):
        model = _ctx(self.file, "Model")
        sub = _ctx(self.file, "Model", identifier="Body", parent=model)
        assert subject.get_context(self.file, "Model", "Body") == sub


# -----------------------------------------------------------------------------
# resolve_representation
# -----------------------------------------------------------------------------


class TestResolveRepresentationIFC4(test.bootstrap.IFC4):
    def test_returns_self_when_not_mapped(self):
        ctx = _ctx(self.file)
        r = _rep(self.file, ctx)
        assert subject.resolve_representation(r) == r

    def test_unwraps_mapped_item(self):
        ctx = _ctx(self.file)
        target = _rep(self.file, ctx)
        mapping_source = self.file.create_entity(
            "IfcRepresentationMap",
            MappingOrigin=self.file.create_entity(
                "IfcAxis2Placement3D",
                Location=self.file.create_entity("IfcCartesianPoint", Coordinates=(0.0, 0.0, 0.0)),
            ),
            MappedRepresentation=target,
        )
        mapped_item = self.file.create_entity(
            "IfcMappedItem",
            MappingSource=mapping_source,
            MappingTarget=self.file.create_entity(
                "IfcCartesianTransformationOperator3D",
                LocalOrigin=self.file.create_entity("IfcCartesianPoint", Coordinates=(0.0, 0.0, 0.0)),
            ),
        )
        wrapper = _rep(self.file, ctx, items=[mapped_item])
        assert subject.resolve_representation(wrapper) == target

    def test_multiple_items_not_unwrapped(self):
        ctx = _ctx(self.file)
        r = _rep(self.file, ctx, items=[
            self.file.create_entity("IfcExtrudedAreaSolid"),
            self.file.create_entity("IfcExtrudedAreaSolid"),
        ])
        assert subject.resolve_representation(r) == r


# -----------------------------------------------------------------------------
# get_representation
# -----------------------------------------------------------------------------


class TestGetRepresentationIFC4(test.bootstrap.IFC4):
    def test_none_when_product_has_no_representation(self):
        wall = self.file.createIfcWall()
        assert subject.get_representation(wall, "Model") is None

    def test_matches_product_by_context_entity(self):
        wall = self.file.createIfcWall()
        ctx = _ctx(self.file)
        r = _rep(self.file, ctx)
        wall.Representation = self.file.create_entity(
            "IfcProductDefinitionShape", Representations=[r]
        )
        assert subject.get_representation(wall, ctx) == r

    def test_matches_product_by_context_type(self):
        wall = self.file.createIfcWall()
        ctx = _ctx(self.file, "Model")
        r = _rep(self.file, ctx)
        wall.Representation = self.file.create_entity(
            "IfcProductDefinitionShape", Representations=[r]
        )
        assert subject.get_representation(wall, "Model") == r

    def test_matches_product_by_subcontext(self):
        wall = self.file.createIfcWall()
        model = _ctx(self.file, "Model")
        sub = _ctx(self.file, "Model", identifier="Body", parent=model)
        other = _rep(self.file, model, identifier="Axis", rep_type="Curve3D")
        body = _rep(self.file, sub)
        wall.Representation = self.file.create_entity(
            "IfcProductDefinitionShape", Representations=[other, body]
        )
        assert subject.get_representation(wall, "Model", "Body") == body

    def test_mismatch_returns_none(self):
        wall = self.file.createIfcWall()
        ctx = _ctx(self.file, "Model")
        r = _rep(self.file, ctx)
        wall.Representation = self.file.create_entity(
            "IfcProductDefinitionShape", Representations=[r]
        )
        assert subject.get_representation(wall, "Plan") is None

    def test_type_product_via_representation_maps(self):
        wt = self.file.createIfcWallType()
        ctx = _ctx(self.file, "Model")
        target = _rep(self.file, ctx)
        rmap = self.file.create_entity(
            "IfcRepresentationMap",
            MappingOrigin=self.file.create_entity(
                "IfcAxis2Placement3D",
                Location=self.file.create_entity("IfcCartesianPoint", Coordinates=(0.0, 0.0, 0.0)),
            ),
            MappedRepresentation=target,
        )
        wt.RepresentationMaps = [rmap]
        assert subject.get_representation(wt, "Model") == target


class TestGetRepresentationIFC2X3(test.bootstrap.IFC2X3):
    def test_matches_by_context_type(self):
        wall = self.file.createIfcWall()
        ctx = _ctx(self.file, "Model")
        r = _rep(self.file, ctx)
        wall.Representation = self.file.create_entity(
            "IfcProductDefinitionShape", Representations=[r]
        )
        assert subject.get_representation(wall, "Model") == r


# -----------------------------------------------------------------------------
# resolve_base_items
# -----------------------------------------------------------------------------


class TestResolveBaseItemsIFC4(test.bootstrap.IFC4):
    def test_plain_items_returned_in_reverse_stack_order(self):
        ctx = _ctx(self.file)
        a = self.file.create_entity("IfcExtrudedAreaSolid")
        b = self.file.create_entity("IfcExtrudedAreaSolid")
        r = _rep(self.file, ctx, items=[a, b])
        got = list(subject.resolve_base_items(r))
        # Upstream pops from end of the queue, so last item yields first.
        assert got == [b, a]

    def test_mapped_item_unwraps_to_inner_items(self):
        ctx = _ctx(self.file)
        leaf = self.file.create_entity("IfcExtrudedAreaSolid")
        target = _rep(self.file, ctx, items=[leaf])
        rmap = self.file.create_entity(
            "IfcRepresentationMap",
            MappingOrigin=self.file.create_entity(
                "IfcAxis2Placement3D",
                Location=self.file.create_entity("IfcCartesianPoint", Coordinates=(0.0, 0.0, 0.0)),
            ),
            MappedRepresentation=target,
        )
        mapped = self.file.create_entity(
            "IfcMappedItem",
            MappingSource=rmap,
            MappingTarget=self.file.create_entity(
                "IfcCartesianTransformationOperator3D",
                LocalOrigin=self.file.create_entity("IfcCartesianPoint", Coordinates=(0.0, 0.0, 0.0)),
            ),
        )
        outer = _rep(self.file, ctx, items=[mapped])
        got = list(subject.resolve_base_items(outer))
        assert got == [leaf]

    def test_boolean_result_expands_operands(self):
        ctx = _ctx(self.file)
        first = self.file.create_entity("IfcExtrudedAreaSolid")
        second = self.file.create_entity("IfcExtrudedAreaSolid")
        boolean = self.file.create_entity(
            "IfcBooleanResult",
            Operator="DIFFERENCE",
            FirstOperand=first,
            SecondOperand=second,
        )
        r = _rep(self.file, ctx, items=[boolean])
        got = set(subject.resolve_base_items(r))
        assert got == {first, second}


# -----------------------------------------------------------------------------
# get_prioritised_contexts
# -----------------------------------------------------------------------------


class TestGetPrioritisedContextsIFC4(test.bootstrap.IFC4):
    def test_empty(self):
        assert subject.get_prioritised_contexts(self.file) == []

    def test_model_before_plan(self):
        model = _ctx(self.file, "Model")
        plan = _ctx(self.file, "Plan")
        got = subject.get_prioritised_contexts(self.file)
        assert got.index(model) < got.index(plan)

    def test_body_identifier_ranks_above_axis(self):
        model = _ctx(self.file, "Model")
        body = _ctx(self.file, "Model", identifier="Body", parent=model)
        axis = _ctx(self.file, "Model", identifier="Axis", parent=model)
        got = subject.get_prioritised_contexts(self.file)
        assert got.index(body) < got.index(axis)

    def test_all_contexts_present(self):
        model = _ctx(self.file, "Model")
        sub = _ctx(self.file, "Model", identifier="Body", parent=model)
        plan = _ctx(self.file, "Plan")
        got = subject.get_prioritised_contexts(self.file)
        assert set(got) == {model, sub, plan}

    def test_target_view_preference(self):
        model = _ctx(self.file, "Model")
        model_view = _ctx(self.file, "Model", identifier="Body",
                          target_view="MODEL_VIEW", parent=model)
        plan_view = _ctx(self.file, "Model", identifier="Body",
                         target_view="PLAN_VIEW", parent=model)
        got = subject.get_prioritised_contexts(self.file)
        assert got.index(model_view) < got.index(plan_view)
