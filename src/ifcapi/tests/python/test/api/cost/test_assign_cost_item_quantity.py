
import ifcopenshell.api.cost
import ifcopenshell.api.control
import ifcopenshell.api.root
import ifcopenshell.guid
import test.bootstrap


class TestAssignCostItemQuantity(test.bootstrap.IFC4):
    def test_assign_product_count(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.cost.assign_cost_item_quantity(self.file, cost_item=item, products=[wall])
        assert item.CostQuantities
        assert item.CostQuantities[0].is_a("IfcQuantityCount")
        assert item.CostQuantities[0][3] == 1

    def test_assign_parametric_quantity(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        slab = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        # Build a QTO manually since pset.add_qto may not exist
        volume_q = self.file.create_entity("IfcQuantityVolume", Name="NetVolume", VolumeValue=42.0)
        qto = self.file.create_entity("IfcElementQuantity", Name="Qto_SlabBaseQuantities", Quantities=[volume_q])
        self.file.create_entity(
            "IfcRelDefinesByProperties",
            GlobalId=ifcopenshell.guid.new(),
            RelatedObjects=[slab],
            RelatingPropertyDefinition=qto,
        )
        ifcopenshell.api.cost.assign_cost_item_quantity(
            self.file, cost_item=item, products=[slab], prop_name="NetVolume"
        )
        assert item.CostQuantities
        found = False
        for q in item.CostQuantities:
            if q.Name == "NetVolume":
                found = True
                assert q[3] == 42.0
        assert found


class TestUnassignCostItemQuantity(test.bootstrap.IFC4):
    def test_unassign_product_count(self):
        schedule = ifcopenshell.api.cost.add_cost_schedule(self.file)
        item = ifcopenshell.api.cost.add_cost_item(self.file, cost_schedule=schedule)
        wall = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.cost.assign_cost_item_quantity(self.file, cost_item=item, products=[wall])
        assert item.CostQuantities
        ifcopenshell.api.cost.unassign_cost_item_quantity(self.file, cost_item=item, products=[wall])
        assert not item.CostQuantities
