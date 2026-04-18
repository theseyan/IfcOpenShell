# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/structural/test_add_structural_analysis_model.py

import ifcopenshell.api.structural
import test.bootstrap


class TestAddStructuralAnalysisModel(test.bootstrap.IFC4):
    def test_adding_a_structural_analysis_model(self):
        subject = ifcopenshell.api.structural.add_structural_analysis_model(self.file)
        models = self.file.by_type("IfcStructuralAnalysisModel")
        assert subject == models[0]
        assert subject.is_a("IfcStructuralAnalysisModel")


class TestAddStructuralAnalysisModelIFC2X3(test.bootstrap.IFC2X3, TestAddStructuralAnalysisModel):
    pass
