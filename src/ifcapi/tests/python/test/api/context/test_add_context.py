# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for context.add_context."""

import ifcopenshell
import ifcopenshell.api.context
import ifcopenshell.api.root
import test.bootstrap


class TestAddContext(test.bootstrap.IFC4):
    def test_add_model_context(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ctx = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        assert ctx.is_a("IfcGeometricRepresentationContext")
        assert ctx.ContextType == "Model"
        assert ctx.CoordinateSpaceDimension == 3
        project = self.file.by_type("IfcProject")[0]
        assert ctx in project.RepresentationContexts

    def test_add_plan_context(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ctx = ifcopenshell.api.context.add_context(self.file, context_type="Plan")
        assert ctx.is_a("IfcGeometricRepresentationContext")
        assert ctx.ContextType == "Plan"
        assert ctx.CoordinateSpaceDimension == 2

    def test_add_subcontext(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        parent = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        sub = ifcopenshell.api.context.add_context(
            self.file, context_type="Model", context_identifier="Body",
            target_view="MODEL_VIEW", parent=parent)
        assert sub.is_a("IfcGeometricRepresentationSubContext")
        assert sub.ContextIdentifier == "Body"
        assert sub.TargetView == "MODEL_VIEW"
        assert sub.ParentContext == parent


class TestAddContextIFC2X3(test.bootstrap.IFC2X3, TestAddContext):
    pass
