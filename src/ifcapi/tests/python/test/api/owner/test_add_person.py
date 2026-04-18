# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAddPerson(test.bootstrap.IFC4):
    def test_adding_a_person(self):
        person = ifcopenshell.api.owner.add_person(
            self.file,
            identification="Identification",
            family_name="FamilyName",
            given_name="GivenName",
        )
        # 0 IfcPerson Identification(>IFC2X3) / Id (IFC2X3)
        assert person[0] == "Identification"
        assert person.FamilyName == "FamilyName"
        assert person.GivenName == "GivenName"


class TestAddPersonIFC2X3(test.bootstrap.IFC2X3, TestAddPerson):
    pass
