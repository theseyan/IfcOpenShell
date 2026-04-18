import ifcopenshell.api.system
import test.bootstrap


class TestAddSystem(test.bootstrap.IFC4):
    def test_adding_a_system(self):
        system = ifcopenshell.api.system.add_system(self.file, ifc_class="IfcSystem")
        system2 = ifcopenshell.api.system.add_system(self.file, ifc_class="IfcDistributionSystem")
        assert system.is_a("IfcSystem")
        if self.file.schema == "IFC2X3":
            assert system2.is_a("IfcSystem")
        else:
            assert system2.is_a("IfcDistributionSystem")


class TestAddSystemIFC2X3(test.bootstrap.IFC2X3, TestAddSystem):
    pass
