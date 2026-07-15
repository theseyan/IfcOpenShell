# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.nest
import ifcopenshell.api.owner.settings
import ifcopenshell.api.root
import pytest

import test.bootstrap


class TestReorderNesting(test.bootstrap.IFC4):
    def setup_items(self):
        parent = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        items = [
            ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
            for _ in range(3)
        ]
        rel = ifcopenshell.api.nest.assign_object(
            self.file, related_objects=items, relating_object=parent
        )
        return rel, items

    def test_default_locates_item_and_moves_to_front(self):
        rel, items = self.setup_items()
        ifcopenshell.api.nest.reorder_nesting(self.file, item=items[2])
        assert rel.RelatedObjects == (items[2], items[0], items[1])

    def test_negative_and_clamped_destinations(self):
        rel, items = self.setup_items()
        ifcopenshell.api.nest.reorder_nesting(
            self.file, item=items[0], old_index=-1, new_index=-99
        )
        assert rel.RelatedObjects == (items[2], items[0], items[1])

    def test_invalid_old_index_is_atomic(self):
        rel, items = self.setup_items()
        before = rel.RelatedObjects
        with pytest.raises((RuntimeError, IndexError)):
            ifcopenshell.api.nest.reorder_nesting(self.file, item=items[0], old_index=9)
        assert rel.RelatedObjects == before

    def test_explicit_nonzero_index_moves_that_entity(self):
        rel, items = self.setup_items()
        ifcopenshell.api.nest.reorder_nesting(
            self.file, item=items[0], old_index=1, new_index=99
        )
        assert rel.RelatedObjects == (items[0], items[2], items[1])

    def test_updating_owner_history(self):
        user = self.file.createIfcPersonAndOrganization()
        application = self.file.createIfcApplication()
        old_user = ifcopenshell.api.owner.settings.get_user
        old_application = ifcopenshell.api.owner.settings.get_application
        ifcopenshell.api.owner.settings.get_user = lambda file: user
        ifcopenshell.api.owner.settings.get_application = lambda file: application
        try:
            rel, items = self.setup_items()
            ifcopenshell.api.nest.reorder_nesting(self.file, item=items[-1])
            assert rel.OwnerHistory.ChangeAction == "MODIFIED"
            assert rel.OwnerHistory.LastModifyingUser == user
            assert rel.OwnerHistory.LastModifyingApplication == application
        finally:
            ifcopenshell.api.owner.settings.get_user = old_user
            ifcopenshell.api.owner.settings.get_application = old_application


class TestReorderNestingIFC2X3(test.bootstrap.IFC2X3, TestReorderNesting):
    pass


class TestReorderNestingIFC4X3(test.bootstrap.IFC4X3, TestReorderNesting):
    pass
