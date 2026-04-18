# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.document
import test.bootstrap


class TestAddReference(test.bootstrap.IFC4):
    def test_adding_a_reference(self):
        element = ifcopenshell.api.document.add_reference(self.file, information=None)
        assert element.is_a("IfcDocumentReference")
        assert len(self.file.by_type("IfcDocumentReference")) == 1

    def test_adding_a_reference_to_an_information(self):
        self.file.createIfcProject()
        information = ifcopenshell.api.document.add_information(self.file, parent=None)
        element = ifcopenshell.api.document.add_reference(self.file, information=information)
        assert element.is_a("IfcDocumentReference")
        assert len(self.file.by_type("IfcDocumentReference")) == 1
        ifc2x3 = self.file.schema == "IFC2X3"
        assert (element.ReferenceToDocument[0] if ifc2x3 else element.ReferencedDocument) == information


class TestAddReferenceIFC2X3(test.bootstrap.IFC2X3, TestAddReference):
    pass
