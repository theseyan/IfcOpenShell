# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestEditActor(test.bootstrap.IFC4):
    def test_editing_an_actor(self):
        person = self.file.createIfcPerson()
        actor = ifcopenshell.api.owner.add_actor(self.file, ifc_class="IfcActor", actor=person)
        ifcopenshell.api.owner.edit_actor(
            self.file,
            actor=actor,
            attributes={"Name": "Name", "Description": "Description", "ObjectType": "ObjectType"},
        )
        assert actor.Name == "Name"
        assert actor.Description == "Description"
        assert actor.ObjectType == "ObjectType"

    def test_editing_an_occupant(self):
        person = self.file.createIfcPerson()
        actor = ifcopenshell.api.owner.add_actor(self.file, ifc_class="IfcOccupant", actor=person)
        ifcopenshell.api.owner.edit_actor(
            self.file,
            actor=actor,
            attributes={
                "Name": "Name",
                "Description": "Description",
                "ObjectType": "ObjectType",
                "PredefinedType": "TENANT",
            },
        )
        assert actor.Name == "Name"
        assert actor.Description == "Description"
        assert actor.ObjectType == "ObjectType"
        assert actor.PredefinedType == "TENANT"


class TestEditActorIFC2X3(test.bootstrap.IFC2X3, TestEditActor):
    pass
