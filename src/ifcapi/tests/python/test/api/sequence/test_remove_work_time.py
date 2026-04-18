# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.sequence
import test.bootstrap


# NOTE: sequence module features relies on entities introduced in IFC4
# therefore no IFC2X3 tests
class TestRemoveWorkTime(test.bootstrap.IFC4):
    def test_remove_work_time(self):
        self.file.create_entity("IfcProject")
        work_calendar = ifcopenshell.api.sequence.add_work_calendar(self.file)
        work_time = ifcopenshell.api.sequence.add_work_time(self.file, work_calendar)
        ifcopenshell.api.sequence.assign_recurrence_pattern(self.file, work_time)
        assert len(self.file.by_type("IfcRecurrencePattern")) == 1

        ifcopenshell.api.sequence.remove_work_time(self.file, work_time)
        assert len(self.file.by_type("IfcWorkTime")) == 0
        assert len(self.file.by_type("IfcRecurrencePattern")) == 0


class TestRemoveWorkTimeIFC4X3(test.bootstrap.IFC4X3, TestRemoveWorkTime):
    pass
