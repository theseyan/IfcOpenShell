# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestUnassignActor(test.bootstrap.IFC4):
    def test_unassigning_an_actor(self):
        wall = self.file.createIfcWall()
        actor = self.file.createIfcActor()
        ifcopenshell.api.owner.assign_actor(self.file, relating_actor=actor, related_object=wall)
        ifcopenshell.api.owner.unassign_actor(self.file, relating_actor=actor, related_object=wall)
        assert len(self.file.by_type("IfcRelAssignsToActor")) == 0


class TestUnassignActorIFC2X3(test.bootstrap.IFC2X3, TestUnassignActor):
    pass
