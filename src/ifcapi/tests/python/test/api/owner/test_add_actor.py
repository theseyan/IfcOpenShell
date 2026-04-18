# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAddActor(test.bootstrap.IFC4):
    def test_adding_an_actor(self):
        person = self.file.createIfcPerson()
        actor = ifcopenshell.api.owner.add_actor(self.file, ifc_class="IfcActor", actor=person)
        assert actor.is_a() == "IfcActor"
        assert actor.TheActor == person

    def test_adding_an_occupant(self):
        person = self.file.createIfcPerson()
        actor = ifcopenshell.api.owner.add_actor(self.file, ifc_class="IfcOccupant", actor=person)
        assert actor.is_a() == "IfcOccupant"
        assert actor.TheActor == person


class TestAddActorIFC2X3(test.bootstrap.IFC2X3, TestAddActor):
    pass
