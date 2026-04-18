# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestRemoveStructuralLoadCase(test.bootstrap.IFC4):
    def test_removing_a_structural_load_case(self):
        load_case = ifcopenshell.api.structural.add_structural_load_case(self.file)
        ifcopenshell.api.structural.remove_structural_load_case(self.file, load_case=load_case)
        assert len(self.file.by_type("IfcStructuralLoadCase")) == 0
