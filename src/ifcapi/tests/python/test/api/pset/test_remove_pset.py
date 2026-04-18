# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for pset.remove_pset — adapted from the original test suite."""

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestRemovePset(test.bootstrap.IFC4):
    def test_removing_a_pset_from_single_product(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"A": "1"})
        ifcopenshell.api.pset.remove_pset(self.file, product=element, pset=pset)
        psets = ifcopenshell.util.element.get_psets(element)
        assert "Foo" not in psets
        assert len(self.file.by_type("IfcPropertySet")) == 0
        assert len(self.file.by_type("IfcRelDefinesByProperties")) == 0

    def test_removing_pset_only_unlinks_from_shared_product(self):
        e1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        e2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=e1, name="Foo")
        ifcopenshell.api.pset.assign_pset(self.file, [e2], pset)
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"A": "1"})

        ifcopenshell.api.pset.remove_pset(self.file, product=e1, pset=pset)
        # Pset should still exist — used by e2
        assert len(self.file.by_type("IfcPropertySet")) == 1
        psets_e2 = ifcopenshell.util.element.get_psets(e2)
        assert "Foo" in psets_e2

    def test_removing_pset_cleans_up_properties(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        pset = ifcopenshell.api.pset.add_pset(self.file, product=element, name="Foo")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"A": "1", "B": 2.0})
        num_props_before = len(self.file.by_type("IfcPropertySingleValue"))
        assert num_props_before == 2
        ifcopenshell.api.pset.remove_pset(self.file, product=element, pset=pset)
        assert len(self.file.by_type("IfcPropertySingleValue")) == 0


class TestRemovePsetIFC2X3(test.bootstrap.IFC2X3, TestRemovePset):
    pass
