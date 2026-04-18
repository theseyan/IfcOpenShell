# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.boundary
import ifcopenshell.api.root
import test.bootstrap


class TestCopyBoundary(test.bootstrap.IFC4):
    def test_run(self):
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")
        boundary2 = ifcopenshell.api.boundary.copy_boundary(self.file, boundary=boundary)
        assert boundary2.is_a("IfcRelSpaceBoundary")
        assert boundary2.GlobalId != boundary.GlobalId

    def test_copying_connection_geometry(self):
        geometry = self.file.createIfcConnectionSurfaceGeometry()
        boundary = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcRelSpaceBoundary")
        boundary.ConnectionGeometry = geometry
        boundary2 = ifcopenshell.api.boundary.copy_boundary(self.file, boundary=boundary)
        assert boundary2.ConnectionGeometry.is_a("IfcConnectionSurfaceGeometry")
        assert boundary2.ConnectionGeometry != boundary.ConnectionGeometry


class TestCopyBoundaryIFC2X3(test.bootstrap.IFC2X3, TestCopyBoundary):
    pass
