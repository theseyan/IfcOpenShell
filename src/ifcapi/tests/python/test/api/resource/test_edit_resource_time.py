# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.resource
import test.bootstrap


class TestEditResourceTime(test.bootstrap.IFC4):
    def test_editing_resource_time(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(self.file, ifc_class="IfcLaborResource")
        time = ifcopenshell.api.resource.add_resource_time(self.file, resource=resource)
        ifcopenshell.api.resource.edit_resource_time(
            self.file, resource_time=time, attributes={"ScheduleWork": "PT16H"}
        )
        assert time.ScheduleWork == "PT16H"
