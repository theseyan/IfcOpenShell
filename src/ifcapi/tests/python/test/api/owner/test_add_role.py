# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAddRole(test.bootstrap.IFC4):
    def test_adding_a_role_to_a_person(self):
        person = self.file.createIfcPerson()
        role = ifcopenshell.api.owner.add_role(self.file, assigned_object=person)
        assert role.is_a("IfcActorRole")
        assert role.Role == "ARCHITECT"
        assert person.Roles == (role,)

    def test_adding_a_role_to_an_organisation(self):
        organisation = self.file.createIfcOrganization()
        role = ifcopenshell.api.owner.add_role(self.file, assigned_object=organisation)
        assert role.is_a("IfcActorRole")
        assert role.Role == "ARCHITECT"
        assert organisation.Roles == (role,)


class TestAddRoleIFC2X3(test.bootstrap.IFC2X3, TestAddRole):
    pass
