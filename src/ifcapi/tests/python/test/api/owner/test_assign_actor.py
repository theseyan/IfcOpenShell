# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAssignActor(test.bootstrap.IFC4):
    def test_assigning_an_actor(self):
        wall = self.file.createIfcWall()
        wall2 = self.file.createIfcWall()
        actor = self.file.createIfcActor()
        ifcopenshell.api.owner.assign_actor(self.file, relating_actor=actor, related_object=wall)
        assert actor.IsActingUpon[0].RelatedObjects == (wall,)
        ifcopenshell.api.owner.assign_actor(self.file, relating_actor=actor, related_object=wall2)
        assert actor.IsActingUpon[0].RelatedObjects == (wall, wall2)

    def test_not_assigning_twice(self):
        wall = self.file.createIfcWall()
        actor = self.file.createIfcActor()
        ifcopenshell.api.owner.assign_actor(self.file, relating_actor=actor, related_object=wall)
        ifcopenshell.api.owner.assign_actor(self.file, relating_actor=actor, related_object=wall)
        assert actor.IsActingUpon[0].RelatedObjects == (wall,)


class TestAssignActorIFC2X3(test.bootstrap.IFC2X3, TestAssignActor):
    pass
