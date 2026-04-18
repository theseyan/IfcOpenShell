# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.layer
import test.bootstrap


class TestAddLayer(test.bootstrap.IFC4):
    def test_add_layer_no_arguments(self):
        layer = ifcopenshell.api.layer.add_layer(self.file)
        assert layer.Name == "Unnamed"

    def test_add_layer_with_name(self):
        layer = ifcopenshell.api.layer.add_layer(self.file, name="Name")
        assert layer.Name == "Name"


class TestAddLayerIFC2X3(test.bootstrap.IFC2X3, TestAddLayer):
    pass
