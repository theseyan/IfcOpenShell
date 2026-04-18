import ifcopenshell.api.library
import ifcopenshell.util.element
import test.bootstrap


class TestUnassignReference(test.bootstrap.IFC4):
    def test_unassigning_a_reference(self):
        reference = self.file.createIfcLibraryReference()
        products = [self.file.createIfcWall() for i in range(3)]
        ifcopenshell.api.library.assign_reference(self.file, products=products, reference=reference)
        ifcopenshell.api.library.unassign_reference(self.file, products=products[:1], reference=reference)
        assert ifcopenshell.util.element.get_referenced_elements(reference) == set(products[1:])
        ifcopenshell.api.library.unassign_reference(self.file, products=products[1:], reference=reference)
        assert ifcopenshell.util.element.get_referenced_elements(reference) == set()
        assert len(self.file.by_type("IfcRelAssociatesLibrary")) == 0


class TestUnassignReferenceIFC2X3(test.bootstrap.IFC2X3, TestUnassignReference):
    pass
