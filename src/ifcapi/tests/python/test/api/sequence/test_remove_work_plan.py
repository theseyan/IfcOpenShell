# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api
import ifcopenshell.api.sequence
import test.bootstrap


def declared_objects(ifc_file: ifcopenshell.file) -> set[ifcopenshell.entity_instance]:
    project = ifc_file.by_type("IfcProject")[0]
    declared = {obj for rel in project.Declares for obj in rel.RelatedDefinitions}
    return declared


# NOTE: sequence module features relies on entities introduced in IFC4
# therefore no IFC2X3 tests
class TestRemoveWorkPlan(test.bootstrap.IFC4):
    def test_remove_work_plan(self):
        self.file.create_entity("IfcProject")
        work_plan = ifcopenshell.api.sequence.add_work_plan(self.file)
        work_schedule = ifcopenshell.api.sequence.add_work_schedule(self.file)
        ifcopenshell.api.sequence.assign_work_plan(self.file, work_schedule, work_plan)
        ifcopenshell.api.sequence.remove_work_plan(self.file, work_plan=work_plan)
        assert len(self.file.by_type("IfcWorkPlan")) == 0
        assert declared_objects(self.file) == set()
        assert len(self.file.by_type("IfcRelAggregates")) == 0


class TestRemoveWorkPlanIFC4X3(test.bootstrap.IFC4X3, TestRemoveWorkPlan):
    pass
