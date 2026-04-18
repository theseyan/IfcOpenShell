# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAddOrganisation(test.bootstrap.IFC4):
    def test_adding_an_organisation(self):
        org = ifcopenshell.api.owner.add_organisation(self.file, identification="Id", name="Name")
        # 0 IfcOrganization Identification(>IFC2X3) / Id (IFC2X3)
        assert org[0] == "Id"
        assert org.Name == "Name"


class TestAddOrganisationIFC2X3(test.bootstrap.IFC2X3, TestAddOrganisation):
    pass
