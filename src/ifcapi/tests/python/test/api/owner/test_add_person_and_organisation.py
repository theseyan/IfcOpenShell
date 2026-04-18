# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAddPersonAndOrganisation(test.bootstrap.IFC4):
    def test_adding(self):
        person = self.file.createIfcPerson()
        organisation = self.file.createIfcOrganization()
        ifcopenshell.api.owner.add_person_and_organisation(self.file, person=person, organisation=organisation)


class TestAddPersonAndOrganisationIFC2X3(test.bootstrap.IFC2X3, TestAddPersonAndOrganisation):
    pass
