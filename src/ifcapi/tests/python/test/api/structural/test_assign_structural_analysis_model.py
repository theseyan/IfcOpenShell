# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/structural/test_assign_structural_analysis_model.py

import ifcopenshell.api.root
import ifcopenshell.api.structural
import test.bootstrap


class TestAssignStructuralAnalysisModel(test.bootstrap.IFC4):
    def test_assigning_a_structural_analysis_model(self):
        subject = ifcopenshell.api.structural.add_structural_analysis_model(self.file)
        product = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcStructuralMember", predefined_type=None, name=None
        )
        rel = ifcopenshell.api.structural.assign_structural_analysis_model(
            self.file,
            products=[product],
            structural_analysis_model=subject,
        )
        assert rel
        assert rel.is_a("IfcRelAssignsToGroup")
        assert rel.RelatingGroup == subject
        assert rel.RelatedObjects == (product,)


class TestAssignStructuralAnalysisModelIFC2X3(test.bootstrap.IFC2X3, TestAssignStructuralAnalysisModel):
    pass
