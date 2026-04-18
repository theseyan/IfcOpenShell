# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestAddStructuralLoadGroup(test.bootstrap.IFC4):
    def test_adding_a_load_group(self):
        load_group = ifcopenshell.api.structural.add_structural_load_group(self.file)
        assert load_group.is_a("IfcStructuralLoadGroup")
        assert load_group.Name == "Unnamed"
        assert load_group.ActionType == "NOTDEFINED"
        assert load_group.ActionSource == "NOTDEFINED"

    def test_adding_a_named_load_group(self):
        load_group = ifcopenshell.api.structural.add_structural_load_group(
            self.file, name="Wind", action_type="VARIABLE_Q", action_source="WIND_W"
        )
        assert load_group.Name == "Wind"
        assert load_group.ActionType == "VARIABLE_Q"
        assert load_group.ActionSource == "WIND_W"
