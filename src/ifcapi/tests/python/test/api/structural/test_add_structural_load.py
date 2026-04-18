# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestAddStructuralLoad(test.bootstrap.IFC4):
    def test_adding_a_structural_load(self):
        load = ifcopenshell.api.structural.add_structural_load(self.file)
        assert load.is_a("IfcStructuralLoadLinearForce")
        assert load.Name is None

    def test_adding_a_named_structural_load(self):
        load = ifcopenshell.api.structural.add_structural_load(self.file, name="My Load")
        assert load.Name == "My Load"

    def test_adding_different_load_types(self):
        load = ifcopenshell.api.structural.add_structural_load(
            self.file, ifc_class="IfcStructuralLoadSingleForce"
        )
        assert load.is_a("IfcStructuralLoadSingleForce")


class TestAddStructuralLoadIFC2X3(test.bootstrap.IFC2X3, TestAddStructuralLoad):
    pass
