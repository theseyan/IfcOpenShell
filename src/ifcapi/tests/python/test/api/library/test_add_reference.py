import ifcopenshell.api.library
import test.bootstrap


class TestAddReference(test.bootstrap.IFC4):
    def test_adding_a_reference(self):
        library = ifcopenshell.api.library.add_library(self.file, name="Name")
        reference = ifcopenshell.api.library.add_reference(self.file, library=library)
        assert reference.is_a("IfcLibraryReference")
        ifc2x3 = self.file.schema == "IFC2X3"
        if ifc2x3:
            assert library.LibraryReference == (reference,)
        else:
            assert reference.ReferencedLibrary == library


class TestAddReferenceIFC2X3(test.bootstrap.IFC2X3, TestAddReference):
    pass
