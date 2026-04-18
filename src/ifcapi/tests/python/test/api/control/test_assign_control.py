
import ifcopenshell.api.control
import ifcopenshell.api.root
import test.bootstrap


class TestAssignControl(test.bootstrap.IFC4):
    def test_run(self):
        wall = self.file.createIfcWall()
        control = self.file.createIfcCostSchedule()

        relation = ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=[wall])
        assert relation
        assert len(self.file.by_type("IfcRelAssignsToControl")) == 1
        assert relation.RelatingControl == control
        assert relation.RelatedObjects == (wall,)

        # Trying to establish existing relationship returns None
        relation = ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=[wall])
        assert relation is None

        # Assigning same control to another object reuses the relationship
        wall1 = self.file.createIfcWall()
        relation = ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=[wall1])
        assert relation is not None
        assert len(self.file.by_type("IfcRelAssignsToControl")) == 1
        assert relation.RelatingControl == control
        assert set(relation.RelatedObjects) == {wall, wall1}

    def test_batch_assignment(self):
        walls = [self.file.createIfcWall() for _ in range(5)]
        control = self.file.createIfcCostSchedule()
        relation = ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=walls)
        assert relation
        assert len(self.file.by_type("IfcRelAssignsToControl")) == 1
        assert relation.RelatingControl == control
        assert set(relation.RelatedObjects) == set(walls)


class TestAssignControlIFC2X3(test.bootstrap.IFC2X3, TestAssignControl):
    pass
