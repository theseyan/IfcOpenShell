# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/structural/test_remove_structural_analysis_model.py

import ifcopenshell.api.structural
import test.bootstrap


class TestRemoveStructuralAnalysisModel(test.bootstrap.IFC4):
    def test_removing_a_structural_analysis_model(self):
        subject = ifcopenshell.api.structural.add_structural_analysis_model(self.file)
        ifcopenshell.api.structural.remove_structural_analysis_model(
            self.file,
            structural_analysis_model=subject,
        )
        models = self.file.by_type("IfcStructuralAnalysisModel")
        assert len(models) == 0


class TestRemoveStructuralAnalysisModelIFC2X3(test.bootstrap.IFC2X3, TestRemoveStructuralAnalysisModel):
    pass
