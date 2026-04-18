# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.library
import test.bootstrap


class TestAssignReference(test.bootstrap.IFC4):
    def test_assigning_a_reference(self):
        reference = self.file.createIfcLibraryReference()
        product = self.file.createIfcWall()
        product2 = self.file.createIfcWall()
        product3 = self.file.createIfcWall()
        ifcopenshell.api.library.assign_reference(self.file, products=[product], reference=reference)
        rel = self.file.by_type("IfcRelAssociatesLibrary")[0]
        assert rel.RelatedObjects == (product,)
        ifcopenshell.api.library.assign_reference(self.file, products=[product2, product3], reference=reference)
        assert set(rel.RelatedObjects) == {product, product2, product3}

    def test_not_assigning_twice(self):
        reference = self.file.createIfcLibraryReference()
        product = self.file.createIfcWall()
        ifcopenshell.api.library.assign_reference(self.file, products=[product], reference=reference)
        ifcopenshell.api.library.assign_reference(self.file, products=[product], reference=reference)
        rel = self.file.by_type("IfcRelAssociatesLibrary")[0]
        assert rel.RelatedObjects == (product,)


class TestAssignReferenceIFC2X3(test.bootstrap.IFC2X3, TestAssignReference):
    pass
