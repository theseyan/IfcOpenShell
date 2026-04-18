# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
# Adapted from src/ifcopenshell-python/test/api/nest/test_unassign_object.py

import ifcopenshell.api.nest
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignObject(test.bootstrap.IFC4):
    def test_unassigning_an_object(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        ifcopenshell.api.nest.assign_object(
            self.file, related_objects=[subelement1, subelement2], relating_object=element
        )
        ifcopenshell.api.nest.unassign_object(self.file, related_objects=[subelement1, subelement2])
        assert ifcopenshell.util.element.get_nest(subelement1) is None
        assert ifcopenshell.util.element.get_nest(subelement2) is None

    def test_the_rel_is_kept_if_there_are_more_nested_elements(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        subelement2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        subelement1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        ifcopenshell.api.nest.assign_object(self.file, related_objects=[subelement1], relating_object=element)
        ifcopenshell.api.nest.assign_object(self.file, related_objects=[subelement2], relating_object=element)
        ifcopenshell.api.nest.unassign_object(self.file, related_objects=[subelement1])
        assert len(self.file.by_type("IfcRelNests")) == 1

    def test_the_rel_is_purged_if_there_are_no_more_nested_elements(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        subelement = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        ifcopenshell.api.nest.assign_object(self.file, related_objects=[subelement], relating_object=element)
        ifcopenshell.api.nest.unassign_object(self.file, related_objects=[subelement])
        assert len(self.file.by_type("IfcRelNests")) == 0

    def test_maintain_assignment_order_in_related_objects(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        subelements = [ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask") for _ in range(5)]
        rel = self.file.create_entity("IfcRelNests", RelatingObject=element, RelatedObjects=subelements)

        original_order = subelements.copy()
        removed_elements = set()
        for i in (0, 3, 2, 4):
            subelement = subelements[i]
            ifcopenshell.api.nest.unassign_object(self.file, related_objects=[subelement])
            removed_elements.add(subelement)
            assert rel.RelatedObjects == tuple([o for o in original_order if o not in removed_elements])


class TestUnassignObjectIFC2X3(test.bootstrap.IFC2X3, TestUnassignObject):
    pass
