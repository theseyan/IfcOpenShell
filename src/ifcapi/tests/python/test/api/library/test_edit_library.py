# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import datetime

import ifcopenshell.api.library
import ifcopenshell.util.date
import pytest

import test.bootstrap


class TestEditLibrary(test.bootstrap.IFC4):
    def test_editing_a_library(self):
        library = self.file.createIfcLibraryInformation()
        dt = datetime.datetime.now()
        attributes = {
            "Name": "Name",
            "Version": "Version",
            "VersionDate": dt,
        }
        if self.file.schema != "IFC2X3":
            attributes["Location"] = "Location"
            attributes["Description"] = "Description"

        ifcopenshell.api.library.edit_library(
            self.file,
            library=library,
            attributes=attributes,
        )
        assert library.Name == "Name"
        assert library.Version == "Version"
        if self.file.schema != "IFC2X3":
            assert library.VersionDate == ifcopenshell.util.date.datetime2ifc(
                dt, "IfcDateTime"
            )
            assert library.Location == "Location"
            assert library.Description == "Description"
        else:
            info = library.VersionDate.get_info()
            del info["id"], info["type"]
            assert info == ifcopenshell.util.date.datetime2ifc(dt, "IfcCalendarDate")

    def test_preserving_microseconds_timezone_and_input_mapping(self):
        library = self.file.createIfcLibraryInformation()
        dt = datetime.datetime(
            2024,
            2,
            29,
            1,
            2,
            3,
            456789,
            tzinfo=datetime.timezone(datetime.timedelta(hours=5, minutes=30)),
        )
        attributes = {"Name": "Library", "VersionDate": dt}

        ifcopenshell.api.library.edit_library(
            self.file, library=library, attributes=attributes
        )

        assert attributes == {"Name": "Library", "VersionDate": dt}
        if self.file.schema != "IFC2X3":
            assert library.VersionDate == "2024-02-29T01:02:03.456789+05:30"
        else:
            assert library.VersionDate.DayComponent == 29
            assert library.VersionDate.MonthComponent == 2
            assert library.VersionDate.YearComponent == 2024

    def test_leaving_schema_compatible_values_on_the_generic_attribute_path(self):
        library = self.file.createIfcLibraryInformation()
        if self.file.schema == "IFC2X3":
            value = self.file.createIfcCalendarDate(29, 2, 2024)
        else:
            value = "2024-02-29T01:02:03"
        ifcopenshell.api.library.edit_library(
            self.file, library=library, attributes={"VersionDate": value}
        )
        assert library.VersionDate == value

    def test_preserving_attribute_order_when_an_earlier_attribute_is_invalid(self):
        library = self.file.createIfcLibraryInformation()
        dt = datetime.datetime(2024, 2, 29, 1, 2, 3)

        with pytest.raises(RuntimeError, match="Attribute Missing not found"):
            ifcopenshell.api.library.edit_library(
                self.file,
                library=library,
                attributes={"Missing": "invalid", "VersionDate": dt},
            )

        assert library.VersionDate is None


class TestEditLibraryIFC2X3(test.bootstrap.IFC2X3, TestEditLibrary):
    pass


class TestEditLibraryIFC4X3(test.bootstrap.IFC4X3, TestEditLibrary):
    pass
