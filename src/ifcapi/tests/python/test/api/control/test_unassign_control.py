
import ifcopenshell.api.control
import ifcopenshell.api.root
import test.bootstrap


class TestUnassignControl(test.bootstrap.IFC4):
    def test_run(self):
        wall = self.file.createIfcWall()
        control = self.file.createIfcCostSchedule()

        relation = ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=[wall])
        assert relation
        ifcopenshell.api.control.unassign_control(
            self.file, relating_control=control, related_objects=[wall])
        assert len(self.file.by_type("IfcRelAssignsToControl")) == 0

    def test_unassign_one_of_many(self):
        wall = self.file.createIfcWall()
        wall1 = self.file.createIfcWall()
        control = self.file.createIfcCostSchedule()

        relation = ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=[wall])
        assert relation
        ifcopenshell.api.control.assign_control(
            self.file, relating_control=control, related_objects=[wall1])
        ifcopenshell.api.control.unassign_control(
            self.file, relating_control=control, related_objects=[wall1])
        assert len(self.file.by_type("IfcRelAssignsToControl")) == 1
        assert relation.RelatedObjects == (wall,)


class TestUnassignControlIFC2X3(test.bootstrap.IFC2X3, TestUnassignControl):
    pass
