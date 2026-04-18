# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestRemoveStructuralLoadGroup(test.bootstrap.IFC4):
    def test_removing_a_structural_load_group(self):
        load_group = ifcopenshell.api.structural.add_structural_load_group(self.file)
        ifcopenshell.api.structural.remove_structural_load_group(self.file, load_group=load_group)
        assert len(self.file.by_type("IfcStructuralLoadGroup")) == 0
