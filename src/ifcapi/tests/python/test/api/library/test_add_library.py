import ifcopenshell.api.library
import test.bootstrap


class TestAddLibrary(test.bootstrap.IFC4):
    def test_adding_a_library(self):
        library = ifcopenshell.api.library.add_library(self.file, name="Name")
        assert library.is_a("IfcLibraryInformation")
        assert library.Name == "Name"


class TestAddLibraryIFC2X3(test.bootstrap.IFC2X3, TestAddLibrary):
    pass
