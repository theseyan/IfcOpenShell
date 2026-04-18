import ifcopenshell.api.layer
import test.bootstrap


class TestAssignLayer(test.bootstrap.IFC4):
    def test_assign_layer_to_items(self):
        items = [self.file.createIfcExtrudedAreaSolid() for i in range(2)]
        layer = self.file.createIfcPresentationLayerAssignment()
        ifcopenshell.api.layer.assign_layer(self.file, items=items, layer=layer)
        assert len(layer.AssignedItems) == 2
        assert set(layer.AssignedItems) == set(items)

    def test_assign_additional_items(self):
        items = [self.file.createIfcExtrudedAreaSolid() for i in range(4)]
        layer = self.file.createIfcPresentationLayerAssignment()
        ifcopenshell.api.layer.assign_layer(self.file, items=items[:2], layer=layer)
        ifcopenshell.api.layer.assign_layer(self.file, items=items[2:], layer=layer)
        assert len(layer.AssignedItems) == 4
        assert set(layer.AssignedItems) == set(items)


class TestAssignLayerIFC2X3(test.bootstrap.IFC2X3, TestAssignLayer):
    pass
