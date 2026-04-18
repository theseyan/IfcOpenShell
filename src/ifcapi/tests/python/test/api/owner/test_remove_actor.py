# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestRemoveActor(test.bootstrap.IFC4):
    def test_removing_an_actor(self):
        person = self.file.createIfcPerson()
        actor = ifcopenshell.api.owner.add_actor(self.file, ifc_class="IfcActor", actor=person)
        ifcopenshell.api.owner.remove_actor(self.file, actor=actor)
        assert len(self.file.by_type("IfcActor")) == 0


class TestRemoveActorIFC2X3(test.bootstrap.IFC2X3, TestRemoveActor):
    pass
