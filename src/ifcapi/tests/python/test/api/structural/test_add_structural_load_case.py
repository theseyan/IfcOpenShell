# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestAddStructuralLoadCase(test.bootstrap.IFC4):
    def test_adding_a_load_case(self):
        load_case = ifcopenshell.api.structural.add_structural_load_case(self.file)
        assert load_case.is_a("IfcStructuralLoadCase")
        assert load_case.Name == "Unnamed"
        assert load_case.ActionType == "NOTDEFINED"
        assert load_case.ActionSource == "NOTDEFINED"

    def test_adding_a_named_load_case(self):
        load_case = ifcopenshell.api.structural.add_structural_load_case(
            self.file, name="Dead Load", action_type="PERMANENT_G", action_source="DEAD_LOAD_G"
        )
        assert load_case.Name == "Dead Load"
        assert load_case.ActionType == "PERMANENT_G"
        assert load_case.ActionSource == "DEAD_LOAD_G"
