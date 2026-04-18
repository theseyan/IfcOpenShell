import pytest

import ifcopenshell.api.classification
import ifcopenshell.api.root
import ifcopenshell.util.classification
import test.bootstrap


class TestRemoveReference(test.bootstrap.IFC4):
    def test_removing_a_reference(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        result = ifcopenshell.api.classification.add_classification(self.file, classification="Name")
        reference = ifcopenshell.api.classification.add_reference(
            self.file,
            products=[element, element2],
            identification="X",
            name="Foobar",
            classification=result,
        )
        ifcopenshell.api.classification.remove_reference(self.file, products=[element, element2], reference=reference)
        assert len(ifcopenshell.util.classification.get_references(element)) == 0
        assert len(ifcopenshell.util.classification.get_references(element2)) == 0
        assert len(self.file.by_type("IfcClassificationReference")) == 0

    def test_retaining_the_reference_if_still_in_use(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element3 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        result = ifcopenshell.api.classification.add_classification(self.file, classification="Name")
        reference = ifcopenshell.api.classification.add_reference(
            self.file,
            products=[element, element2, element3],
            identification="X",
            name="Foobar",
            classification=result,
        )
        assert len(self.file.by_type("IfcClassificationReference")) == 1
        ifcopenshell.api.classification.remove_reference(self.file, products=[element, element2], reference=reference)
        assert len(self.file.by_type("IfcClassificationReference")) == 1
        ifcopenshell.api.classification.remove_reference(self.file, products=[element3], reference=reference)
        assert len(self.file.by_type("IfcClassificationReference")) == 0


class TestRemoveReferenceIFC2X3(test.bootstrap.IFC2X3, TestRemoveReference):
    pass
