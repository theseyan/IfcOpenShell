# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestEditRole(test.bootstrap.IFC4):
    def test_editing_a_role(self):
        role = self.file.createIfcActorRole()
        ifcopenshell.api.owner.edit_role(
            self.file,
            role=role,
            attributes={"Role": "ARCHITECT", "UserDefinedRole": "UserDefinedRole", "Description": "Description"},
        )
        assert role.Role == "ARCHITECT"
        assert role.UserDefinedRole == "UserDefinedRole"
        assert role.Description == "Description"


class TestEditRoleIFC2X3(test.bootstrap.IFC2X3, TestEditRole):
    pass
