# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import pytest

import test.bootstrap


class TestAddResourceTime(test.bootstrap.IFC4):
    def test_adding_resource_time(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        time = ifcopenshell.api.resource.add_resource_time(self.file, resource=resource)
        assert time.is_a("IfcResourceTime")
        assert resource.Usage == time

    def test_replacing_usage_preserves_the_upstream_orphan(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        first = ifcopenshell.api.resource.add_resource_time(
            self.file, resource=resource
        )
        second = ifcopenshell.api.resource.add_resource_time(
            self.file, resource=resource
        )
        assert resource.Usage == second
        assert first in self.file


class TestAddResourceTimeIFC2X3(test.bootstrap.IFC2X3):
    def test_failing_before_creating_an_unavailable_resource_time(self):
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        entity_count = len(list(self.file))
        with pytest.raises(RuntimeError, match="IfcResourceTime"):
            ifcopenshell.api.resource.add_resource_time(self.file, resource=resource)
        assert len(list(self.file)) == entity_count


class TestAddResourceTimeIFC4X3(test.bootstrap.IFC4X3, TestAddResourceTime):
    pass
