# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestEditAddress(test.bootstrap.IFC4):
    def test_editing_a_postal_address(self):
        address = self.file.createIfcPostalAddress()
        ifcopenshell.api.owner.edit_address(
            self.file,
            address=address,
            attributes={
                "Purpose": "OFFICE",
                "Description": "Description",
                "UserDefinedPurpose": "UserDefinedPurpose",
                "InternalLocation": "InternalLocation",
                "AddressLines": ["Address", "Lines"],
                "PostalBox": "PostalBox",
                "Town": "Town",
                "Region": "Region",
                "PostalCode": "PostalCode",
                "Country": "Country",
            },
        )
        assert address.Purpose == "OFFICE"
        assert address.Description == "Description"
        assert address.UserDefinedPurpose == "UserDefinedPurpose"
        assert address.InternalLocation == "InternalLocation"
        assert address.AddressLines == ("Address", "Lines")
        assert address.PostalBox == "PostalBox"
        assert address.Town == "Town"
        assert address.Region == "Region"
        assert address.PostalCode == "PostalCode"
        assert address.Country == "Country"

    def test_editing_a_telecom_address(self):
        address = self.file.createIfcTelecomAddress()
        attributes = {
            "Purpose": "OFFICE",
            "Description": "Description",
            "UserDefinedPurpose": "UserDefinedPurpose",
            "TelephoneNumbers": ["Telephone", "Numbers"],
            "FacsimileNumbers": ["Facsimile", "Numbers"],
            "PagerNumber": "PagerNumber",
            "ElectronicMailAddresses": ["Electronic", "Mail", "Addresses"],
            "WWWHomePageURL": "WWWHomePageURL",
        }
        if self.file.schema != "IFC2X3":
            attributes["MessagingIDs"] = ["Messaging", "IDs"]

        ifcopenshell.api.owner.edit_address(
            self.file,
            address=address,
            attributes=attributes,
        )
        assert address.Purpose == "OFFICE"
        assert address.Description == "Description"
        assert address.UserDefinedPurpose == "UserDefinedPurpose"
        assert address.TelephoneNumbers == ("Telephone", "Numbers")
        assert address.FacsimileNumbers == ("Facsimile", "Numbers")
        assert address.PagerNumber == "PagerNumber"
        assert address.ElectronicMailAddresses == ("Electronic", "Mail", "Addresses")
        assert address.WWWHomePageURL == "WWWHomePageURL"
        if self.file.schema != "IFC2X3":
            assert address.MessagingIDs == ("Messaging", "IDs")


class TestEditAddressIFC2X3(test.bootstrap.IFC2X3, TestEditAddress):
    pass
