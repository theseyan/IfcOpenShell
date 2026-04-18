# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.root
import ifcopenshell.api.system
import ifcopenshell.util.system
import test.bootstrap


class TestUnassignSystem(test.bootstrap.IFC4):
    def test_unassign_system(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcFlowSegment")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcFlowSegment")
        element3 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcFlowSegment")
        system = ifcopenshell.api.system.add_system(self.file)
        ifcopenshell.api.system.assign_system(self.file, products=[element, element2, element3], system=system)
        ifcopenshell.api.system.unassign_system(self.file, products=[element2, element3], system=system)
        assert ifcopenshell.util.system.get_system_elements(system) == [element]

        ifcopenshell.api.system.unassign_system(self.file, products=[element], system=system)
        assert ifcopenshell.util.system.get_system_elements(system) == []


class TestUnassignSystemIFC2X3(test.bootstrap.IFC2X3, TestUnassignSystem):
    pass
