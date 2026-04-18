# This file was generated with the assistance of an AI coding tool.

import pytest

import ifcopenshell.api.context
import ifcopenshell.api.georeference
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestRemoveGeoreferencing(test.bootstrap.IFC4):
    def test_removing_georeferencing(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ifcopenshell.api.context.add_context(self.file, "Model")
        ifcopenshell.api.georeference.add_georeferencing(self.file)
        ifcopenshell.api.georeference.remove_georeferencing(self.file)
        assert len(self.file.by_type("IfcMapConversion")) == 0
        assert len(self.file.by_type("IfcProjectedCRS")) == 0


class TestRemoveGeoreferencingIFC2X3(test.bootstrap.IFC2X3):
    def test_removing_georeferencing(self):
        project = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ifcopenshell.api.georeference.add_georeferencing(self.file)
        ifcopenshell.api.georeference.remove_georeferencing(self.file)
        conversion = ifcopenshell.util.element.get_pset(project, "ePSet_MapConversion", verbose=True)
        crs = ifcopenshell.util.element.get_pset(project, "ePSet_ProjectedCRS", verbose=True)
        assert not conversion
        assert not crs
