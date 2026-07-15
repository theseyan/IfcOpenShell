# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import ifcopenshell.api.constraint
import ifcopenshell.api.pset
import ifcopenshell.api.resource
import ifcopenshell.api.root
import ifcopenshell.api.sequence

import test.bootstrap


# NOTE: resource module features relies on entities introduced in IFC4
# therefore no IFC2X3 tests
class TestCalculateResourceWork(test.bootstrap.IFC4):
    def assign_count_work(self, resource, product_count=1):
        task = ifcopenshell.api.sequence.add_task(self.file)
        for _ in range(product_count):
            product = ifcopenshell.api.root.create_entity(
                self.file, ifc_class="IfcSlab"
            )
            ifcopenshell.api.sequence.assign_product(
                self.file, relating_product=product, related_object=task
            )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )

    def add_productivity(self, resource, properties):
        pset = ifcopenshell.api.pset.add_pset(
            self.file, product=resource, name="EPset_Productivity"
        )
        ifcopenshell.api.pset.edit_pset(self.file, pset=pset, properties=properties)
        return pset

    def test_calculating_resource_work_based_on_a_daily_productivity_rate(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")

        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        pset = ifcopenshell.api.pset.add_pset(
            self.file, product=resource, name="EPset_Productivity"
        )
        ifcopenshell.api.pset.edit_pset(
            self.file,
            pset=pset,
            properties={
                "BaseQuantityConsumed": "P0.5D",
                "BaseQuantityProducedName": "GrossVolume",
                "BaseQuantityProducedValue": 5,
            },
        )

        slab = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        qto = ifcopenshell.api.pset.add_qto(
            self.file, product=slab, name="Qto_SlabBaseQuantities"
        )
        ifcopenshell.api.pset.edit_qto(
            self.file, qto=qto, properties={"GrossVolume": 20}
        )

        task = ifcopenshell.api.sequence.add_task(self.file)
        ifcopenshell.api.sequence.assign_product(
            self.file, relating_product=slab, related_object=task
        )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert resource.Usage.ScheduleWork == "P2.0D"

    def test_calculating_resource_work_based_on_an_hourly_productivity_rate(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")

        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        pset = ifcopenshell.api.pset.add_pset(
            self.file, product=resource, name="EPset_Productivity"
        )
        ifcopenshell.api.pset.edit_pset(
            self.file,
            pset=pset,
            properties={
                "BaseQuantityConsumed": "PT1H",
                "BaseQuantityProducedName": "GrossVolume",
                "BaseQuantityProducedValue": 5,
            },
        )

        slab = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        qto = ifcopenshell.api.pset.add_qto(
            self.file, product=slab, name="Qto_SlabBaseQuantities"
        )
        ifcopenshell.api.pset.edit_qto(
            self.file, qto=qto, properties={"GrossVolume": 20}
        )

        task = ifcopenshell.api.sequence.add_task(self.file)
        ifcopenshell.api.sequence.assign_product(
            self.file, relating_product=slab, related_object=task
        )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert resource.Usage.ScheduleWork == "PT4.0H"

    def test_no_calculation_if_no_productivity_data_available(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")

        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )

        slab = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        qto = ifcopenshell.api.pset.add_qto(
            self.file, product=slab, name="Qto_SlabBaseQuantities"
        )
        ifcopenshell.api.pset.edit_qto(
            self.file, qto=qto, properties={"GrossVolume": 20}
        )

        task = ifcopenshell.api.sequence.add_task(self.file)
        ifcopenshell.api.sequence.assign_product(
            self.file, relating_product=slab, related_object=task
        )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)

        assert resource.Usage is None

        pset = ifcopenshell.api.pset.add_pset(
            self.file, product=resource, name="EPset_Productivity"
        )
        ifcopenshell.api.pset.edit_pset(
            self.file, pset=pset, properties={"BaseQuantityProducedName": "foo"}
        )

        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)

        assert resource.Usage is None

    def test_calculating_resource_work_based_on_a_counted_quantity(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")

        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        pset = ifcopenshell.api.pset.add_pset(
            self.file, product=resource, name="EPset_Productivity"
        )
        ifcopenshell.api.pset.edit_pset(
            self.file,
            pset=pset,
            properties={
                "BaseQuantityConsumed": "PT1H",
                "BaseQuantityProducedName": "Count",
                "BaseQuantityProducedValue": 2,
            },
        )

        slab = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        qto = ifcopenshell.api.pset.add_qto(
            self.file, product=slab, name="Qto_SlabBaseQuantities"
        )
        ifcopenshell.api.pset.edit_qto(
            self.file, qto=qto, properties={"GrossVolume": 20}
        )

        ifcopenshell.api.sequence.assign_product(
            self.file, relating_product=slab, related_object=resource
        )

        schedule = ifcopenshell.api.sequence.add_work_schedule(self.file)
        task = ifcopenshell.api.sequence.add_task(self.file, work_schedule=schedule)
        ifcopenshell.api.sequence.assign_product(
            self.file, relating_product=slab, related_object=task
        )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert resource.Usage.ScheduleWork == "PT0.5H"

    def test_inheriting_only_the_direct_parents_productivity(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        parent = ifcopenshell.api.resource.add_resource(self.file)
        resource = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent, ifc_class="IfcLaborResource"
        )
        self.add_productivity(
            parent,
            {
                "BaseQuantityConsumed": "PT2H",
                "BaseQuantityProducedName": "Count",
                "BaseQuantityProducedValue": 1,
            },
        )
        self.assign_count_work(resource)
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert resource.Usage.ScheduleWork == "PT2.0H"

    def test_not_merging_an_incomplete_child_pset_with_its_parent(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        parent = ifcopenshell.api.resource.add_resource(self.file)
        resource = ifcopenshell.api.resource.add_resource(
            self.file, parent_resource=parent, ifc_class="IfcLaborResource"
        )
        self.add_productivity(
            parent,
            {
                "BaseQuantityConsumed": "PT2H",
                "BaseQuantityProducedName": "Count",
                "BaseQuantityProducedValue": 1,
            },
        )
        self.add_productivity(resource, {"BaseQuantityProducedName": "Count"})
        self.assign_count_work(resource)
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert resource.Usage is None

    def test_summing_first_matching_numeric_value_for_each_product(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        self.add_productivity(
            resource,
            {
                "BaseQuantityConsumed": "PT1H",
                "BaseQuantityProducedName": "GrossVolume",
                "BaseQuantityProducedValue": 10,
            },
        )
        task = ifcopenshell.api.sequence.add_task(self.file)
        first = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        first_pset = ifcopenshell.api.pset.add_pset(
            self.file, product=first, name="Custom"
        )
        ifcopenshell.api.pset.edit_pset(
            self.file, pset=first_pset, properties={"GrossVolume": 3.0}
        )
        first_qto = ifcopenshell.api.pset.add_qto(
            self.file, product=first, name="Qto_SlabBaseQuantities"
        )
        ifcopenshell.api.pset.edit_qto(
            self.file, qto=first_qto, properties={"GrossVolume": 100.0}
        )
        second = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcSlab")
        second_qto = ifcopenshell.api.pset.add_qto(
            self.file, product=second, name="Qto_SlabBaseQuantities"
        )
        ifcopenshell.api.pset.edit_qto(
            self.file, qto=second_qto, properties={"GrossVolume": 7.0}
        )
        for product in (first, second):
            ifcopenshell.api.sequence.assign_product(
                self.file, relating_product=product, related_object=task
            )
        ifcopenshell.api.sequence.assign_process(
            self.file, relating_process=task, related_object=resource
        )
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert resource.Usage.ScheduleWork == "PT1.0H"

    def test_zero_productivity_leaves_existing_usage_unchanged(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        usage = ifcopenshell.api.resource.add_resource_time(
            self.file, resource=resource
        )
        usage.ScheduleWork = "PT9H"
        self.add_productivity(
            resource,
            {
                "BaseQuantityConsumed": "PT1H",
                "BaseQuantityProducedName": "Count",
                "BaseQuantityProducedValue": 0,
            },
        )
        self.assign_count_work(resource)
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert usage.ScheduleWork == "PT9H"

    def test_hard_equal_constraint_locks_schedule_work(self):
        ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcProject")
        resource = ifcopenshell.api.resource.add_resource(
            self.file, ifc_class="IfcLaborResource"
        )
        usage = ifcopenshell.api.resource.add_resource_time(
            self.file, resource=resource
        )
        usage.ScheduleWork = "PT9H"
        self.add_productivity(
            resource,
            {
                "BaseQuantityConsumed": "PT1H",
                "BaseQuantityProducedName": "Count",
                "BaseQuantityProducedValue": 1,
            },
        )
        objective = ifcopenshell.api.constraint.add_objective(self.file)
        metric = ifcopenshell.api.constraint.add_metric(self.file, objective=objective)
        metric.ConstraintGrade = "HARD"
        metric.Benchmark = "EQUALTO"
        metric.ReferencePath = self.file.create_entity(
            "IfcReference", AttributeIdentifier="Usage.ScheduleWork"
        )
        ifcopenshell.api.constraint.assign_constraint(
            self.file, products=[resource], constraint=objective
        )
        self.assign_count_work(resource)
        ifcopenshell.api.resource.calculate_resource_work(self.file, resource=resource)
        assert usage.ScheduleWork == "PT9H"
