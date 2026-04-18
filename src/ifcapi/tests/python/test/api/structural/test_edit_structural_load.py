# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestEditStructuralLoad(test.bootstrap.IFC4):
    def test_editing_a_structural_load(self):
        load = ifcopenshell.api.structural.add_structural_load(
            self.file, ifc_class="IfcStructuralLoadSingleForce"
        )
        ifcopenshell.api.structural.edit_structural_load(
            self.file, structural_load=load, attributes={"Name": "My Load"}
        )
        assert load.Name == "My Load"


class TestEditStructuralLoadIFC2X3(test.bootstrap.IFC2X3, TestEditStructuralLoad):
    pass
