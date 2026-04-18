import ifcopenshell.api.library
import test.bootstrap


class TestEditReference(test.bootstrap.IFC4):
    def test_editing_a_reference(self):
        reference = self.file.createIfcLibraryReference()
        attr_name = "Identification" if self.file.schema != "IFC2X3" else "ItemReference"
        ifcopenshell.api.library.edit_reference(
            self.file, reference=reference,
            attributes={"Location": "Location", attr_name: "Identification", "Name": "Name"})
        assert reference.Location == "Location"
        assert reference[1] == "Identification"
        assert reference.Name == "Name"


class TestEditReferenceIFC2X3(test.bootstrap.IFC2X3, TestEditReference):
    pass
