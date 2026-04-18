
import pytest
import numpy as np

import ifcopenshell.api.context
import ifcopenshell.api.georeference
import ifcopenshell.api.root
import ifcopenshell.util.geolocation
import test.bootstrap


class TestEditTrueNorth(test.bootstrap.IFC4):
    def test_editing_true_north(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        model = ifcopenshell.api.context.add_context(self.file, "Model")
        plan = ifcopenshell.api.context.add_context(self.file, "Plan")
        ifcopenshell.api.georeference.edit_true_north(self.file, true_north=[0.0, 1.0])
        assert model.TrueNorth[0] == (0.0, 1.0)
        assert plan.TrueNorth[0] == (0.0, 1.0)
        ifcopenshell.api.georeference.edit_true_north(self.file, true_north=[-0.5, 0.8660254])
        assert np.isclose(ifcopenshell.util.geolocation.get_true_north(self.file), 30)
        ifcopenshell.api.georeference.edit_true_north(self.file, true_north=30)
        assert np.isclose(ifcopenshell.util.geolocation.get_true_north(self.file), 30)
