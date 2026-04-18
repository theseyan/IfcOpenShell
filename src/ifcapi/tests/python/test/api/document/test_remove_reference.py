import ifcopenshell.api.document
import test.bootstrap


class TestRemoveReference(test.bootstrap.IFC4):
    def test_removing_reference(self):
        project = self.file.createIfcProject()
        information = ifcopenshell.api.document.add_information(self.file, parent=None)
        reference = ifcopenshell.api.document.add_reference(self.file, information=information)
        ifcopenshell.api.document.remove_reference(self.file, reference=reference)
        assert len(self.file.by_type("IfcDocumentReference")) == 0
        assert len(self.file.by_type("IfcDocumentInformation")) == 1
        assert len(self.file.by_type("IfcRelAssociatesDocument")) == 1

    def test_removing_a_reference_assigned_to_an_object(self):
        project = self.file.createIfcProject()
        wall = self.file.createIfcWall()
        information = ifcopenshell.api.document.add_information(self.file, parent=None)
        reference = ifcopenshell.api.document.add_reference(self.file, information=information)
        ifcopenshell.api.document.assign_document(self.file, products=[wall], document=reference)
        assert len(self.file.by_type("IfcRelAssociatesDocument")) == 2
        ifcopenshell.api.document.remove_reference(self.file, reference=reference)
        assert len(self.file.by_type("IfcDocumentReference")) == 0
        assert len(self.file.by_type("IfcDocumentInformation")) == 1
        assert len(self.file.by_type("IfcRelAssociatesDocument")) == 1


class TestRemoveReferenceIFC2X3(test.bootstrap.IFC2X3, TestRemoveReference):
    pass
