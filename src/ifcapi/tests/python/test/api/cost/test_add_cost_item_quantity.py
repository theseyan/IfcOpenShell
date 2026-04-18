
import ifcopenshell.api.control
import ifcopenshell.api.cost
import ifcopenshell.api.root
import test.bootstrap


class TestAddCostItemQuantity(test.bootstrap.IFC4):
    def test_add_quantity_count(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.control.assign_control(self.file, relating_control=item, related_objects=[wall])
        quantity = ifcopenshell.api.cost.add_cost_item_quantity(self.file, cost_item=item, ifc_class="IfcQuantityCount")
        assert quantity.is_a("IfcQuantityCount")
        assert quantity.Name == "Unnamed"
        assert quantity[3] == 1

    def test_add_quantity_volume(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        quantity = ifcopenshell.api.cost.add_cost_item_quantity(
            self.file, cost_item=item, ifc_class="IfcQuantityVolume"
        )
        assert quantity.is_a("IfcQuantityVolume")
        assert quantity[3] == 0.0
        assert item.CostQuantities == (quantity,)
