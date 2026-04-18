# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.layer
import test.bootstrap


class TestUnassignLayer(test.bootstrap.IFC4):
    def test_unassign_layer_from_items(self):
        items = [self.file.createIfcExtrudedAreaSolid() for i in range(3)]
        layer = self.file.createIfcPresentationLayerAssignment()
        ifcopenshell.api.layer.assign_layer(self.file, items=items, layer=layer)
        ifcopenshell.api.layer.unassign_layer(self.file, items=items[2:], layer=layer)
        assert len(layer.AssignedItems) == 2
        assert set(layer.AssignedItems) == set(items[:2])

    def test_remove_layer_if_all_items_are_unassigned(self):
        items = [self.file.createIfcExtrudedAreaSolid() for i in range(3)]
        layer = self.file.createIfcPresentationLayerAssignment()
        ifcopenshell.api.layer.assign_layer(self.file, items=items, layer=layer)
        ifcopenshell.api.layer.unassign_layer(self.file, items=items, layer=layer)
        assert not self.file.by_type("IfcPresentationLayerAssignment")


class TestUnassignLayerIFC2X3(test.bootstrap.IFC2X3, TestUnassignLayer):
    pass
