# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.nest
import ifcopenshell.api.sequence
import test.bootstrap


# NOTE: sequence module features relies on entities introduced in IFC4
# therefore no IFC2X3 tests
class TestRemoveTask(test.bootstrap.IFC4):
    def test_remove_task(self):
        self.file.create_entity("IfcProject")
        work_schedule = ifcopenshell.api.sequence.add_work_schedule(self.file)
        task = ifcopenshell.api.sequence.add_task(self.file, work_schedule=work_schedule)
        ifcopenshell.api.sequence.remove_task(self.file, task)
        assert len(self.file.by_type("IfcTask")) == 0

    def test_remove_task_times(self):
        self.file.create_entity("IfcProject")

        task = ifcopenshell.api.sequence.add_task(self.file)
        task_time = ifcopenshell.api.sequence.add_task_time(self.file, task, is_recurring=True)
        ifcopenshell.api.sequence.assign_recurrence_pattern(self.file, task_time, recurrence_type="DAILY")

        task2 = ifcopenshell.api.sequence.add_task(self.file)
        task_time = ifcopenshell.api.sequence.add_task_time(self.file, task2, is_recurring=False)

        ifcopenshell.api.sequence.remove_task(self.file, task)
        ifcopenshell.api.sequence.remove_task(self.file, task2)

        assert len(self.file.by_type("IfcTask")) == 0
        assert len(self.file.by_type("IfcTaskTime")) == 0
        assert len(self.file.by_type("IfcRecurrencePattern")) == 0

    def test_remove_task_with_subtasks(self):
        self.file.create_entity("IfcProject")
        work_schedule = ifcopenshell.api.sequence.add_work_schedule(self.file)
        task = ifcopenshell.api.sequence.add_task(self.file, work_schedule=work_schedule)
        subtask1 = ifcopenshell.api.sequence.add_task(self.file, work_schedule=work_schedule)
        subtask2 = ifcopenshell.api.sequence.add_task(self.file, work_schedule=work_schedule)
        ifcopenshell.api.nest.assign_object(self.file, related_objects=[subtask1, subtask2], relating_object=task)
        ifcopenshell.api.sequence.remove_task(self.file, task)
        assert len(self.file.by_type("IfcTask")) == 0
        assert len(self.file.by_type("IfcRelNests")) == 0

    def test_remove_subtask(self):
        self.file.create_entity("IfcProject")
        work_schedule = ifcopenshell.api.sequence.add_work_schedule(self.file)
        task = ifcopenshell.api.sequence.add_task(self.file, work_schedule=work_schedule)
        subtask1 = ifcopenshell.api.sequence.add_task(self.file, work_schedule=work_schedule)
        ifcopenshell.api.nest.assign_object(self.file, related_objects=[subtask1], relating_object=task)
        ifcopenshell.api.sequence.remove_task(self.file, subtask1)
        assert len(self.file.by_type("IfcTask")) == 1
        assert len(self.file.by_type("IfcRelNests")) == 0


class TestRemoveTaskIFC4X3(test.bootstrap.IFC4X3, TestRemoveTask):
    pass
