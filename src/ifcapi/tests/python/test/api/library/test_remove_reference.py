# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.library
import test.bootstrap


class TestRemoveReference(test.bootstrap.IFC4):
    def test_removing_a_reference(self):
        reference = self.file.createIfcLibraryReference()
        product = self.file.createIfcWall()
        ifcopenshell.api.library.assign_reference(self.file, products=[product], reference=reference)
        ifcopenshell.api.library.remove_reference(self.file, reference=reference)
        assert len(self.file.by_type("IfcLibraryReference")) == 0
        assert len(self.file.by_type("IfcRelAssociatesLibrary")) == 0


class TestRemoveReferenceIFC2X3(test.bootstrap.IFC2X3, TestRemoveReference):
    pass
