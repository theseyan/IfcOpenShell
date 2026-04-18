# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import test.bootstrap


class TestRemoveStructuralLoad(test.bootstrap.IFC4):
    def test_removing_a_structural_load(self):
        load = ifcopenshell.api.structural.add_structural_load(self.file)
        ifcopenshell.api.structural.remove_structural_load(self.file, structural_load=load)
        assert len(self.file.by_type("IfcStructuralLoadLinearForce")) == 0


class TestRemoveStructuralLoadIFC2X3(test.bootstrap.IFC2X3, TestRemoveStructuralLoad):
    pass
