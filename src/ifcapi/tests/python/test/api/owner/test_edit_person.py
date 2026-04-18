# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestEditPerson(test.bootstrap.IFC4):
    def test_editing_a_person(self):
        person = self.file.createIfcPerson()
        ifcopenshell.api.owner.edit_person(
            self.file,
            person=person,
            attributes={
                "Identification" if self.file.schema != "IFC2X3" else "Id": "Identification",
                "FamilyName": "FamilyName",
                "GivenName": "GivenName",
                "MiddleNames": ["Middle", "Names"],
                "PrefixTitles": ["Prefix", "Titles"],
                "SuffixTitles": ["Suffix", "Titles"],
            },
        )
        # 0 IfcPerson Identification(>IFC2X3) / Id (IFC2X3)
        assert person[0] == "Identification"
        assert person.FamilyName == "FamilyName"
        assert person.GivenName == "GivenName"
        assert person.MiddleNames == ("Middle", "Names")
        assert person.PrefixTitles == ("Prefix", "Titles")
        assert person.SuffixTitles == ("Suffix", "Titles")


class TestEditPersonIFC2X3(test.bootstrap.IFC2X3, TestEditPerson):
    pass
