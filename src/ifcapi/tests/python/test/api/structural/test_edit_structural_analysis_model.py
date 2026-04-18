# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/structural/test_edit_structural_analysis_model.py

import ifcopenshell.api.structural
import test.bootstrap


class TestEditStructuralAnalysisModel(test.bootstrap.IFC4):
    def test_editing_a_structural_analysis_model(self):
        subject = ifcopenshell.api.structural.add_structural_analysis_model(self.file)
        ifcopenshell.api.structural.edit_structural_analysis_model(
            self.file,
            structural_analysis_model=subject,
            attributes={"Name": "My edited model", "Description": "Description of my model"},
        )
        models = self.file.by_type("IfcStructuralAnalysisModel")
        assert subject == models[0]
        assert subject.is_a("IfcStructuralAnalysisModel")


class TestEditStructuralAnalysisModelIFC2X3(test.bootstrap.IFC2X3, TestEditStructuralAnalysisModel):
    pass
