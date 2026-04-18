
import pytest
import numpy as np

import ifcopenshell.api.context
import ifcopenshell.api.georeference
import ifcopenshell.api.root
import ifcopenshell.util.geolocation
import test.bootstrap


class TestEditWCS(test.bootstrap.IFC4):
    def test_editing_wcs(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ifcopenshell.api.context.add_context(self.file, "Model")
        ifcopenshell.api.context.add_context(self.file, "Plan")

        ifcopenshell.api.georeference.edit_wcs(self.file)
        wcs = ifcopenshell.util.geolocation.get_wcs(self.file)
        m = np.eye(4)
        assert np.allclose(wcs, m)

        ifcopenshell.api.georeference.edit_wcs(self.file, x=1, y=2, z=3)
        m[:, 3] = [1, 2, 3, 1]
        wcs = ifcopenshell.util.geolocation.get_wcs(self.file)
        assert np.allclose(wcs, m)

        ifcopenshell.api.georeference.edit_wcs(self.file, x=1, y=2, z=3, rotation=90)
        m[:, 0] = [0, 1, 0, 0]
        m[:, 1] = [-1, 0, 0, 0]
        wcs = ifcopenshell.util.geolocation.get_wcs(self.file)
        assert np.allclose(wcs, m)
