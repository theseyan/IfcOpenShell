# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.sequence
import test.bootstrap


class TestNativeSequenceCore(test.bootstrap.IFC4):
    def test_adding_task_time(self):
        task = self.file.createIfcTask()
        task_time = ifcopenshell.api.sequence.add_task_time(self.file, task=task)
        assert task_time.is_a("IfcTaskTime")
        assert task.TaskTime == task_time

    def test_adding_recurring_task_time(self):
        task = self.file.createIfcTask()
        task_time = ifcopenshell.api.sequence.add_task_time(self.file, task=task, is_recurring=True)
        assert task_time.is_a("IfcTaskTimeRecurring")
        assert task.TaskTime == task_time

    def test_assigning_a_process_returns_none_when_already_assigned(self):
        task = self.file.createIfcTask()
        wall = self.file.createIfcWall()
        rel = ifcopenshell.api.sequence.assign_process(self.file, relating_process=task, related_object=wall)
        assert rel.is_a("IfcRelAssignsToProcess")
        assert ifcopenshell.api.sequence.assign_process(self.file, relating_process=task, related_object=wall) is None
        assert task.OperatesOn[0].RelatedObjects == (wall,)

    def test_unassigning_a_process_partially_and_fully(self):
        task = self.file.createIfcTask()
        wall = self.file.createIfcWall()
        wall2 = self.file.createIfcWall()
        rel = ifcopenshell.api.sequence.assign_process(self.file, relating_process=task, related_object=wall)
        ifcopenshell.api.sequence.assign_process(self.file, relating_process=task, related_object=wall2)
        result = ifcopenshell.api.sequence.unassign_process(self.file, relating_process=task, related_object=wall)
        assert result == rel
        assert rel.RelatedObjects == (wall2,)
        result = ifcopenshell.api.sequence.unassign_process(self.file, relating_process=task, related_object=wall2)
        assert result is None
        assert len(self.file.by_type("IfcRelAssignsToProcess")) == 0

    def test_assigning_a_product_returns_existing_assignment_when_already_assigned(self):
        wall = self.file.createIfcWall()
        task = self.file.createIfcTask()
        rel = ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task)
        assert ifcopenshell.api.sequence.assign_product(self.file, relating_product=wall, related_object=task) == rel
        assert wall.ReferencedBy[0].RelatedObjects == (task,)

    def test_assigning_and_unassigning_lag_time(self):
        task = self.file.createIfcTask()
        task2 = self.file.createIfcTask()
        rel_sequence = self.file.createIfcRelSequence(RelatingProcess=task, RelatedProcess=task2)
        lag = ifcopenshell.api.sequence.assign_lag_time(self.file, rel_sequence=rel_sequence, lag_value="P1D")
        assert lag.DurationType == "WORKTIME"
        assert lag.LagValue.wrappedValue == "P1D"
        assert rel_sequence.TimeLag == lag
        ifcopenshell.api.sequence.unassign_lag_time(self.file, rel_sequence=rel_sequence)
        assert rel_sequence.TimeLag is None
        assert len(self.file.by_type("IfcLagTime")) == 0

    def test_editing_task_sequence_and_lag_attributes(self):
        task = self.file.createIfcTask()
        task2 = self.file.createIfcTask()
        rel_sequence = self.file.createIfcRelSequence(RelatingProcess=task, RelatedProcess=task2)
        lag = ifcopenshell.api.sequence.assign_lag_time(self.file, rel_sequence=rel_sequence, lag_value="P1D")
        ifcopenshell.api.sequence.edit_task(self.file, task=task, attributes={"Identification": "A"})
        ifcopenshell.api.sequence.edit_sequence(
            self.file, rel_sequence=rel_sequence, attributes={"SequenceType": "START_START"}
        )
        ifcopenshell.api.sequence.edit_lag_time(self.file, lag_time=lag, attributes={"LagValue": 0.5})
        assert task.Identification == "A"
        assert rel_sequence.SequenceType == "START_START"
        assert lag.LagValue.is_a("IfcRatioMeasure")
        assert lag.LagValue.wrappedValue == 0.5
