import ifcopenshell.api.classification
import ifcopenshell.api.root
import test.bootstrap


class TestRemoveClassification(test.bootstrap.IFC4):
    def test_removing_a_classification(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        element = ifcopenshell.api.classification.add_classification(self.file, classification="Name")
        ifcopenshell.api.classification.remove_classification(self.file, classification=element)
        assert not self.file.by_type("IfcClassification")

    def test_removing_a_classification_and_all_of_its_references(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        result = ifcopenshell.api.classification.add_classification(self.file, classification="Name")
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.classification.add_reference(
            self.file,
            products=[element],
            identification="X",
            name="Foobar",
            classification=result,
        )
        ifcopenshell.api.classification.remove_classification(self.file, classification=result)
        assert not self.file.by_type("IfcClassification")
        assert not self.file.by_type("IfcClassificationReference")
        assert not self.file.by_type("IfcRelAssociatesClassification")


class TestRemoveClassificationIFC2X3(test.bootstrap.IFC2X3, TestRemoveClassification):
    pass
