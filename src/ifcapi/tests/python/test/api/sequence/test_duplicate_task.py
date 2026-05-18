# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.pset
import ifcopenshell.api.root
import ifcopenshell.api.sequence
import ifcopenshell.util.element
import ifcopenshell.util.sequence
import test.bootstrap


class TestDuplicateTask(test.bootstrap.IFC4):
    def test_copying_nested_tasks_psets_and_sequence_relationships(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        schedule = ifcopenshell.api.sequence.add_work_schedule(self.file)
        task = ifcopenshell.api.sequence.add_task(
            self.file, work_schedule=schedule, name="Root", identification="A"
        )
        subtask = ifcopenshell.api.sequence.add_task(
            self.file, parent_task=task, name="Child", identification="A.1"
        )
        pset = ifcopenshell.api.pset.add_pset(self.file, product=task, name="Custom")
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties={"Foo": "Bar"})
        rel_sequence = ifcopenshell.api.sequence.assign_sequence(
            self.file, relating_process=task, related_process=subtask, sequence_type="START_START"
        )
        ifcopenshell.api.sequence.assign_lag_time(self.file, rel_sequence=rel_sequence, lag_value="P2D")

        current, duplicate = ifcopenshell.api.sequence.duplicate_task(self.file, task)

        assert current == [task, subtask]
        assert len(duplicate) == 2
        assert duplicate[0] != task
        assert duplicate[1] != subtask
        assert duplicate[0].Name == "Root"
        assert duplicate[0].Identification == "A"
        assert duplicate[0].GlobalId != task.GlobalId
        assert duplicate[1].Nests[0].RelatingObject == duplicate[0]
        assert ifcopenshell.util.element.get_pset(duplicate[0], "Custom", "Foo") == "Bar"
        assert pset.id() != ifcopenshell.util.element.get_pset(duplicate[0], "Custom")["id"]
        sequence_rels = [
            rel
            for rel in self.file.by_type("IfcRelSequence")
            if rel.RelatingProcess == duplicate[0] and rel.RelatedProcess == duplicate[1]
        ]
        assert len(sequence_rels) == 1
        assert sequence_rels[0].SequenceType == "START_START"
        assert sequence_rels[0].TimeLag.LagValue.wrappedValue == "P2D"
        assert duplicate[0] not in ifcopenshell.util.sequence.get_work_schedule_tasks(schedule)


class TestCreateBaseline(test.bootstrap.IFC4):
    def test_creating_a_baseline_schedule_and_task_references(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        schedule = ifcopenshell.api.sequence.add_work_schedule(
            self.file, name="Planned", predefined_type="PLANNED"
        )
        task = ifcopenshell.api.sequence.add_task(self.file, work_schedule=schedule, name="Root")
        subtask = ifcopenshell.api.sequence.add_task(self.file, parent_task=task, name="Child")

        ifcopenshell.api.sequence.create_baseline(self.file, schedule, name="Baseline 1")

        baseline = [s for s in self.file.by_type("IfcWorkSchedule") if s.PredefinedType == "BASELINE"][0]
        assert baseline.Name == "Baseline 1"
        assert schedule.Declares[0].RelatedObjects == (baseline,)
        baseline_tasks = list(ifcopenshell.util.sequence.get_work_schedule_tasks(baseline))
        assert len(baseline_tasks) == 2
        assert not set(baseline_tasks).intersection({task, subtask})
        assert task.Declares[0].RelatedObjects == (baseline_tasks[0],)
        assert subtask.Declares[0].RelatedObjects == (baseline_tasks[1],)

    def test_not_creating_a_baseline_for_non_planned_schedules(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        schedule = ifcopenshell.api.sequence.add_work_schedule(self.file, predefined_type="ACTUAL")

        ifcopenshell.api.sequence.create_baseline(self.file, schedule, name="Baseline 1")

        assert len([s for s in self.file.by_type("IfcWorkSchedule") if s.PredefinedType == "BASELINE"]) == 0

    def test_creating_a_baseline_with_no_name(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        schedule = ifcopenshell.api.sequence.add_work_schedule(self.file, predefined_type="PLANNED")

        ifcopenshell.api.sequence.create_baseline(self.file, schedule)

        baseline = [s for s in self.file.by_type("IfcWorkSchedule") if s.PredefinedType == "BASELINE"][0]
        assert baseline.Name is None
