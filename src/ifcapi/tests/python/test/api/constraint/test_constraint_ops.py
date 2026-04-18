import ifcopenshell.api.constraint
import test.bootstrap


class TestAddObjective(test.bootstrap.IFC4):
    def test_adding_an_objective(self):
        objective = ifcopenshell.api.constraint.add_objective(self.file)
        assert objective.is_a("IfcObjective")
        assert objective.Name == "Unnamed"
        assert objective.ConstraintGrade == "NOTDEFINED"


class TestAddObjectiveIFC2X3(test.bootstrap.IFC2X3, TestAddObjective):
    pass


class TestAddMetric(test.bootstrap.IFC4):
    def test_adding_a_metric(self):
        objective = ifcopenshell.api.constraint.add_objective(self.file)
        metric = ifcopenshell.api.constraint.add_metric(self.file, objective=objective)
        assert metric.is_a("IfcMetric")
        assert metric in objective.BenchmarkValues

    def test_adding_a_standalone_metric(self):
        metric = ifcopenshell.api.constraint.add_metric(self.file, objective=None)
        assert metric.is_a("IfcMetric")


class TestAddMetricIFC2X3(test.bootstrap.IFC2X3, TestAddMetric):
    pass


class TestRemoveConstraint(test.bootstrap.IFC4):
    def test_removing_a_constraint(self):
        objective = ifcopenshell.api.constraint.add_objective(self.file)
        wall = self.file.createIfcWall()
        ifcopenshell.api.constraint.assign_constraint(self.file, products=[wall], constraint=objective)
        ifcopenshell.api.constraint.remove_constraint(self.file, constraint=objective)
        assert len(self.file.by_type("IfcObjective")) == 0
        assert len(self.file.by_type("IfcRelAssociatesConstraint")) == 0


class TestRemoveConstraintIFC2X3(test.bootstrap.IFC2X3, TestRemoveConstraint):
    pass


class TestRemoveMetric(test.bootstrap.IFC4):
    def test_removing_a_metric(self):
        objective = ifcopenshell.api.constraint.add_objective(self.file)
        metric = ifcopenshell.api.constraint.add_metric(self.file, objective=objective)
        ifcopenshell.api.constraint.remove_metric(self.file, metric=metric)
        assert len(self.file.by_type("IfcMetric")) == 0


class TestRemoveMetricIFC2X3(test.bootstrap.IFC2X3, TestRemoveMetric):
    pass
