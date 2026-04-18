# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.library
import test.bootstrap


class TestRemoveLibrary(test.bootstrap.IFC4):
    def test_removing_a_library(self):
        library = self.file.createIfcLibraryInformation()
        ifcopenshell.api.library.remove_library(self.file, library=library)
        assert len(self.file.by_type("IfcLibraryInformation")) == 0

    def test_removing_a_library_and_all_references(self):
        if self.file.schema != "IFC2X3":
            library = self.file.createIfcLibraryInformation()
            reference1 = self.file.createIfcLibraryReference(ReferencedLibrary=library)
            reference2 = self.file.createIfcLibraryReference(ReferencedLibrary=library)
        else:
            reference1 = self.file.createIfcLibraryReference()
            reference2 = self.file.createIfcLibraryReference()
            library = self.file.createIfcLibraryInformation(LibraryReference=[reference1, reference2])
        self.file.createIfcRelAssociatesLibrary(GlobalId="foo", RelatingLibrary=library)
        self.file.createIfcRelAssociatesLibrary(GlobalId="bar", RelatingLibrary=reference1)
        ifcopenshell.api.library.remove_library(self.file, library=library)
        assert len(self.file.by_type("IfcLibraryReference")) == 0
        assert len(self.file.by_type("IfcRelAssociatesLibrary")) == 0


class TestRemoveLibraryIFC2X3(test.bootstrap.IFC2X3, TestRemoveLibrary):
    pass
