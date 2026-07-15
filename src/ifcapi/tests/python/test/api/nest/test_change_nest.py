# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.nest
import ifcopenshell.api.root

import test.bootstrap


def nests_from(entity):
    relationships = getattr(entity, "IsNestedBy", None)
    if relationships is None:
        relationships = entity.IsDecomposedBy
    return tuple(rel for rel in relationships if rel.is_a("IfcRelNests"))


def nests_to(entity):
    relationships = getattr(entity, "Nests", None)
    if relationships is None:
        relationships = entity.Decomposes
    return tuple(rel for rel in relationships if rel.is_a("IfcRelNests"))


class TestChangeNest(test.bootstrap.IFC4):
    def test_moves_child_and_preserves_order(self):
        old = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        new = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        first = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        item = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        target_child = ifcopenshell.api.root.create_entity(
            self.file, ifc_class="IfcTask"
        )
        ifcopenshell.api.nest.assign_object(
            self.file, related_objects=[first, item], relating_object=old
        )
        ifcopenshell.api.nest.assign_object(
            self.file, related_objects=[target_child], relating_object=new
        )
        ifcopenshell.api.nest.change_nest(self.file, item=item, new_parent=new)
        assert nests_from(old)[0].RelatedObjects == (first,)
        assert nests_from(new)[0].RelatedObjects == (target_child, item)

    def test_not_nested_is_noop(self):
        item = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        parent = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        ifcopenshell.api.nest.change_nest(self.file, item=item, new_parent=parent)
        assert not nests_from(parent)

    def test_moving_to_the_same_parent_moves_to_the_end(self):
        parent = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        children = [
            ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
            for _ in range(3)
        ]
        relation = ifcopenshell.api.nest.assign_object(
            self.file, related_objects=children, relating_object=parent
        )
        ifcopenshell.api.nest.change_nest(
            self.file, item=children[0], new_parent=parent
        )
        assert relation.RelatedObjects == (children[1], children[2], children[0])

    def test_recreating_a_relationship_for_a_sole_child(self):
        old_parent = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        new_parent = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        item = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcTask")
        old_relation = ifcopenshell.api.nest.assign_object(
            self.file, related_objects=[item], relating_object=old_parent
        )
        old_id = old_relation.id()
        ifcopenshell.api.nest.change_nest(self.file, item=item, new_parent=new_parent)
        assert nests_to(item)[0].RelatingObject == new_parent
        assert nests_to(item)[0].id() != old_id


class TestChangeNestIFC2X3(test.bootstrap.IFC2X3, TestChangeNest):
    pass


class TestChangeNestIFC4X3(test.bootstrap.IFC4X3, TestChangeNest):
    pass
