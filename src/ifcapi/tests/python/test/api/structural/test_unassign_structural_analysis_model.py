# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/structural/test_unassign_structural_analysis_model.py

import ifcopenshell.api.root
import ifcopenshell.api.structural
import test.bootstrap


class TestUnassignStructuralAnalysisModel(test.bootstrap.IFC4):
    def test_unassigning_a_structural_analysis_model(self):
        subject = ifcopenshell.api.structural.add_structural_analysis_model(self.file)
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcStructuralMember", predefined_type=None, name=None
        )
        ifcopenshell.api.structural.assign_structural_analysis_model(
            self.file,
            products=[product],
            structural_analysis_model=subject,
        )
        ifcopenshell.api.structural.unassign_structural_analysis_model(
            self.file,
            products=[product],
            structural_analysis_model=subject,
        )
        models = self.file.by_type("IfcStructuralAnalysisModel")
        rels = self.file.by_type("IfcRelAssignsToGroup")
        assert len(models[0].IsGroupedBy) == 0
        assert len(rels) == 0


class TestUnassignStructuralAnalysisModelIFC2X3(test.bootstrap.IFC2X3, TestUnassignStructuralAnalysisModel):
    pass
