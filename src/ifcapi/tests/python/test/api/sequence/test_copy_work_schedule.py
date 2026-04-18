# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.sequence
import ifcopenshell.util.sequence
import test.bootstrap


class TestCopyWorkSchedule(test.bootstrap.IFC4):
    def test_run(self):
        self.file.create_entity("IfcProject")
        # Shared code logic with test_copy_cost_schedule.
        work_plan = ifcopenshell.api.sequence.add_work_plan(self.file)
        schedule = ifcopenshell.api.sequence.add_work_schedule(self.file, work_plan=work_plan)
        task = ifcopenshell.api.sequence.add_task(self.file, work_schedule=schedule)
        ifcopenshell.api.sequence.add_task(self.file, parent_task=task)  # Subtask.
        old_cost_items = set(self.file.by_type("IfcTask"))

        new_schedule = ifcopenshell.api.sequence.copy_work_schedule(self.file, schedule)

        # We don't check how well IfcTasks are copied,
        # it should be tested separately in test_duplicate_task.
        assert isinstance(new_schedule, ifcopenshell.entity_instance)
        assert new_schedule != schedule
        assert len(ifcopenshell.util.sequence.get_root_tasks(new_schedule)) == 1
        new_tasks = set(ifcopenshell.util.sequence.get_work_schedule_tasks(new_schedule))
        assert len(new_tasks) == 2
        assert len(new_tasks.intersection(old_cost_items)) == 0


class TestCopyWorkScheduleIFC2X3(test.bootstrap.IFC2X3, TestCopyWorkSchedule):
    pass


class TestCopyWorkScheduleIFC4X3(test.bootstrap.IFC4X3, TestCopyWorkSchedule):
    pass
