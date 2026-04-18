# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.document
import test.bootstrap


class TestAddInformation(test.bootstrap.IFC4):
    def test_adding_information(self):
        project = self.file.createIfcProject()
        element = ifcopenshell.api.document.add_information(self.file, parent=None)
        assert element.is_a("IfcDocumentInformation")
        assert len(self.file.by_type("IfcDocumentInformation")) == 1

    def test_adding_information_to_the_project(self):
        project = self.file.createIfcProject()
        element = ifcopenshell.api.document.add_information(self.file, parent=None)
        rel = self.file.by_type("IfcRelAssociatesDocument")[0]
        assert rel.is_a("IfcRelAssociatesDocument")
        assert rel.RelatingDocument == element
        assert rel.RelatedObjects == (project,)

    def test_adding_a_subdocument(self):
        project = self.file.createIfcProject()
        parent = ifcopenshell.api.document.add_information(self.file, parent=None)
        element = ifcopenshell.api.document.add_information(self.file, parent=parent)
        assert element.is_a("IfcDocumentInformation")
        assert len(self.file.by_type("IfcDocumentInformation")) == 2
        assert element.IsPointedTo[0].RelatingDocument == parent
        assert parent.IsPointer[0].RelatedDocuments[0] == element
        element2 = ifcopenshell.api.document.add_information(self.file, parent=parent)
        assert element in parent.IsPointer[0].RelatedDocuments
        assert element2 in parent.IsPointer[0].RelatedDocuments


class TestAddInformationIFC2X3(test.bootstrap.IFC2X3, TestAddInformation):
    pass
