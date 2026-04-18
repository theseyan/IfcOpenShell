# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.classification
import ifcopenshell.api.root
import test.bootstrap


class TestAddClassification(test.bootstrap.IFC4):
    def test_adding_a_classification(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ifcopenshell.api.classification.add_classification(self.file, classification="Name")
        assert self.file.by_type("IfcClassification")[0].Name == "Name"

    def test_adding_a_classification_from_a_library(self):
        library = ifcopenshell.file()
        classification = library.createIfcClassification(Name="Name")
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        ifcopenshell.api.classification.add_classification(self.file, classification=classification)
        assert self.file.by_type("IfcClassification")[0].Name == "Name"


class TestAddClassificationIFC2X3(test.bootstrap.IFC2X3, TestAddClassification):
    pass
