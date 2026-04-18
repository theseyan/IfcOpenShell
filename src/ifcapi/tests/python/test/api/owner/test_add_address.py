# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestAddAddress(test.bootstrap.IFC4):
    def test_adding_to_a_person(self):
        person = self.file.createIfcPerson()
        postal = ifcopenshell.api.owner.add_address(self.file, assigned_object=person, ifc_class="IfcPostalAddress")
        telecom = ifcopenshell.api.owner.add_address(self.file, assigned_object=person, ifc_class="IfcTelecomAddress")
        assert postal.is_a("IfcPostalAddress")
        assert telecom.is_a("IfcTelecomAddress")
        assert postal.Purpose == telecom.Purpose == "OFFICE"
        assert postal in person.Addresses
        assert telecom in person.Addresses

    def test_adding_to_a_organisation(self):
        organisation = self.file.createIfcOrganization()
        postal = ifcopenshell.api.owner.add_address(
            self.file, assigned_object=organisation, ifc_class="IfcPostalAddress"
        )
        telecom = ifcopenshell.api.owner.add_address(
            self.file, assigned_object=organisation, ifc_class="IfcTelecomAddress"
        )
        assert postal.is_a("IfcPostalAddress")
        assert telecom.is_a("IfcTelecomAddress")
        assert postal.Purpose == telecom.Purpose == "OFFICE"
        assert postal in organisation.Addresses
        assert telecom in organisation.Addresses


class TestAddAddressIFC2X3(test.bootstrap.IFC2X3, TestAddAddress):
    pass
