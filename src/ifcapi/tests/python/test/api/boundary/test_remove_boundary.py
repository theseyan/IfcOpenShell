
import ifcopenshell.api.boundary
import ifcopenshell.api.root
import test.bootstrap


class TestRemoveBoundary(test.bootstrap.IFC4):
    def test_run(self):
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")
        ifcopenshell.api.boundary.remove_boundary(self.file, boundary=boundary)
        assert not self.file.by_type("IfcRelSpaceBoundary")

    def test_removing_connection_geometry(self):
        geometry = self.file.createIfcConnectionSurfaceGeometry()
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")
        boundary.ConnectionGeometry = geometry
        ifcopenshell.api.boundary.remove_boundary(self.file, boundary=boundary)
        assert not self.file.by_type("IfcRelSpaceBoundary")
        assert not self.file.by_type("IfcConnectionSurfaceGeometry")


class TestRemoveBoundaryIFC2X3(test.bootstrap.IFC2X3, TestRemoveBoundary):
    pass
