# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.constraint
import ifcopenshell.api.resource
import ifcopenshell.api.sequence
import pytest

import test.bootstrap


class TestCalculateResourceUsage(test.bootstrap.IFC4):
    def setup_usage(self, task_duration="P1D", duration_type=None, work="PT16H"):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        usage = ifcopenshell.api.resource.add_resource_time(
            self.file, resource=resource
        )
        usage.ScheduleWork = work
        task = ifcopenshell.api.sequence.add_task(self.file)
        task.TaskTime = self.file.create_entity(
            "IfcTaskTime", ScheduleDuration=task_duration, DurationType=duration_type
        )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        return resource, usage

    @pytest.mark.parametrize(
        ("duration_type", "expected"),
        [(None, 2.0), ("WORKTIME", 2.0), ("ELAPSEDTIME", 2 / 3)],
    )
    def test_day_duration_types(self, duration_type, expected):
        resource, usage = self.setup_usage(duration_type=duration_type)
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage == pytest.approx(expected)

    def test_hours_and_fractional_duration(self):
        resource, usage = self.setup_usage(task_duration="PT4.5H", work="PT9H")
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage == pytest.approx(2.0)

    def test_missing_prerequisites_are_noops(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert resource.Usage is None

    def test_each_missing_task_prerequisite_is_a_noop(self):
        self.file.create_entity("IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        usage = ifcopenshell.api.resource.add_resource_time(
            self.file, resource=resource
        )
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage is None
        usage.ScheduleWork = "PT8H"
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage is None
        task = ifcopenshell.api.sequence.add_task(self.file)
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage is None

    def test_using_the_first_task_assignment(self):
        resource, usage = self.setup_usage(task_duration="P1D", work="PT8H")
        second = ifcopenshell.api.sequence.add_task(self.file)
        second.TaskTime = self.file.create_entity(
            "IfcTaskTime", ScheduleDuration="PT1H"
        )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=second, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage == pytest.approx(1.0)

    def test_parsing_day_based_schedule_work(self):
        resource, usage = self.setup_usage(task_duration="P1D", work="P0.5D")
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage == pytest.approx(1.5)

    def test_hard_equal_constraint_locks_schedule_usage(self):
        resource, usage = self.setup_usage()
        usage.ScheduleUsage = 3.0
        objective = ifcopenshell.api.constraint.add_objective(self.file)
        metric = ifcopenshell.api.constraint.add_metric(self.file, objective=objective)
        metric.ConstraintGrade = "HARD"
        metric.Benchmark = "EQUALTO"
        metric.ReferencePath = self.file.create_entity(
            "IfcReference", AttributeIdentifier="Usage.ScheduleUsage"
        )
        ifcopenshell.api.constraint.assign_constraint(
            self.file, products=[resource], constraint=objective
        )
        ifcopenshell.api.resource.calculate_resource_usage(self.file, resource=resource)
        assert usage.ScheduleUsage == 3.0

    @pytest.mark.parametrize("duration", ["bad", "P0D"])
    def test_invalid_task_duration_is_atomic(self, duration):
        resource, usage = self.setup_usage(task_duration=duration)
        usage.ScheduleUsage = 3.0
        with pytest.raises((RuntimeError, ValueError)):
            ifcopenshell.api.resource.calculate_resource_usage(
                self.file, resource=resource
            )
        assert usage.ScheduleUsage == 3.0
