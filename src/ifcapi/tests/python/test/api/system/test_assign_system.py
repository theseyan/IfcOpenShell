import pytest

import ifcopenshell.api.root
import ifcopenshell.api.system
import ifcopenshell.util.system
import test.bootstrap


class TestAssignSystem(test.bootstrap.IFC4):
    def test_assign_system(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcFlowSegment")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcFlowSegment")
        system = ifcopenshell.api.system.add_system(self.file)
        ifcopenshell.api.system.assign_system(self.file, products=[element, element2], system=system)
        assert len(self.file.by_type("IfcRelAssignsToGroup")) == 1
        assert set(ifcopenshell.util.system.get_system_elements(system)) == set(self.file.by_type("IfcFlowSegment"))

    def test_exception_on_unassignable_elements(self):
        element = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcFlowSegment")
        proj = self.file.createIfcProject()
        system = ifcopenshell.api.system.add_system(self.file)
        with pytest.raises(TypeError):
            ifcopenshell.api.system.assign_system(self.file, products=[element, proj], system=system)
        with pytest.raises(TypeError):
            ifcopenshell.api.system.assign_system(self.file, products=[element], system=proj)


class TestAssignSystemIFC2X3(test.bootstrap.IFC2X3, TestAssignSystem):
    pass
