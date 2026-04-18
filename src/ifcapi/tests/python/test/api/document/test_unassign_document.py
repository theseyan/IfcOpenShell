import ifcopenshell.api.document
import ifcopenshell.api.root
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignDocument(test.bootstrap.IFC4):
    def test_unassigning_a_document(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        reference = ifcopenshell.api.document.add_reference(self.file, information=None)
        ifcopenshell.api.document.assign_document(self.file, products=[element], document=reference)
        ifcopenshell.api.document.unassign_document(self.file, products=[element], document=reference)
        assert not element.HasAssociations
        assert not len(self.file.by_type("IfcRelAssociatesDocument"))

    def test_unassigning_a_document_used_by_multiple_entities(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element3 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        reference = ifcopenshell.api.document.add_reference(self.file, information=None)
        ifcopenshell.api.document.assign_document(self.file, products=[element, element2, element3], document=reference)
        ifcopenshell.api.document.unassign_document(self.file, products=[element, element2], document=reference)
        assert ifcopenshell.util.element.get_referenced_elements(reference) == {element3}


class TestUnassignDocumentIFC2X3(test.bootstrap.IFC2X3, TestUnassignDocument):
    pass
