# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.library
import test.bootstrap


class TestEditLibrary(test.bootstrap.IFC4):
    def test_editing_a_library(self):
        library = self.file.createIfcLibraryInformation()
        ifcopenshell.api.library.edit_library(
            self.file, library=library, attributes={"Name": "Name", "Version": "Version"})
        assert library.Name == "Name"
        assert library.Version == "Version"


class TestEditLibraryIFC2X3(test.bootstrap.IFC2X3, TestEditLibrary):
    pass
