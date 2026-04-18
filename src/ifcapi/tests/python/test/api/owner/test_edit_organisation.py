# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestEditOrganisation(test.bootstrap.IFC4):
    def test_editing_a_organisation(self):
        organisation = self.file.createIfcOrganization()
        attributes = {
            "Identification" if self.file.schema != "IFC2X3" else "Id": "Identification",
            "Name": "Name",
            "Description": "Description",
        }
        ifcopenshell.api.owner.edit_organisation(
            self.file,
            organisation=organisation,
            attributes=attributes,
        )
        for attr, value in attributes.items():
            assert getattr(organisation, attr) == value


class TestEditOrganisationIFC2X3(test.bootstrap.IFC2X3, TestEditOrganisation):
    pass
