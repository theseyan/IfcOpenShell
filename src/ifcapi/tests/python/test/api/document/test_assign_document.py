# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.document
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestAssignDocument(test.bootstrap.IFC4):
    def test_assigning_a_document(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        reference = ifcopenshell.api.document.add_reference(self.file, information=None)
        ifcopenshell.api.document.assign_document(self.file, products=[element], document=reference)
        assert element.HasAssociations[0].RelatingDocument == reference
        assert ifcopenshell.util.element.get_referenced_elements(reference) == {element}

    def test_assigning_multiple_documents(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        reference = ifcopenshell.api.document.add_reference(self.file, information=None)
        ifcopenshell.api.document.assign_document(self.file, products=[element, element2], document=reference)
        assert len(self.file.by_type("IfcRelAssociatesDocument")) == 1
        assert ifcopenshell.util.element.get_referenced_elements(reference) == {element, element2}


class TestAssignDocumentIFC2X3(test.bootstrap.IFC2X3, TestAssignDocument):
    pass
