import pytest

import ifcopenshell.api.classification
import ifcopenshell.api.root
import ifcopenshell.util.classification
import test.bootstrap


class TestAddReference(test.bootstrap.IFC4):
    def test_adding_a_reference(self):
        is_ifc2x3 = self.file.schema == "IFC2X3"
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        result = ifcopenshell.api.classification.add_classification(self.file, classification="Name")
        ifcopenshell.api.classification.add_reference(
            self.file,
            products=[element, element2],
            identification="X",
            name="Foobar",
            classification=result,
        )
        references = list(ifcopenshell.util.classification.get_references(element))
        assert len(references) == 1
        assert getattr(references[0], "ItemReference" if is_ifc2x3 else "Identification") == "X"
        assert references[0].Name == "Foobar"
        assert references[0].ReferencedSource == self.file.by_type("IfcClassification")[0]

        references2 = list(ifcopenshell.util.classification.get_references(element))
        assert len(references2) == 1
        assert references2[0] == references[0]

        rel = next(
            rel
            for rel in self.file.by_type("IfcRelAssociatesClassification")
            if rel.RelatingClassification == references[0]
        )
        assert len(rel.RelatedObjects) == 2

    def test_adding_a_reference_to_a_resource_and_to_a_root(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        element = self.file.createIfcMaterial()
        element2 = self.file.createIfcCostValue()
        element3 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        result = ifcopenshell.api.classification.add_classification(self.file, classification="Name")

        if self.file.schema == "IFC2X3":
            with pytest.raises(TypeError):
                ifcopenshell.api.classification.add_reference(
                    self.file,
                    products=[element, element2, element3],
                    identification="X",
                    name="Foobar",
                    classification=result,
                )
            return

        ifcopenshell.api.classification.add_reference(
            self.file,
            products=[element, element2, element3],
            identification="X",
            name="Foobar",
            classification=result,
        )

        references = list(ifcopenshell.util.classification.get_references(element))
        assert len(references) == 1
        assert references[0].Identification == "X"
        assert references[0].ReferencedSource == self.file.by_type("IfcClassification")[0]

        references3 = list(ifcopenshell.util.classification.get_references(element3))
        assert references3[0] == references[0]

        assert len(self.file.by_type("IfcExternalReferenceRelationship")[0].RelatedResourceObjects) == 2
        rel = next(
            rel
            for rel in self.file.by_type("IfcRelAssociatesClassification")
            if rel.RelatingClassification == references[0]
        )
        assert len(rel.RelatedObjects) == 1


class TestAddReferenceIFC2X3(test.bootstrap.IFC2X3, TestAddReference):
    pass
