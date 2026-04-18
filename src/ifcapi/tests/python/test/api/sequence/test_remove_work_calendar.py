# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.control
import ifcopenshell.api.sequence
import test.bootstrap


# NOTE: sequence module features relies on entities introduced in IFC4
# therefore no IFC2X3 tests
class TestRemoveWorkCalendar(test.bootstrap.IFC4):
    def test_remove_work_calendar(self):
        self.file.create_entity("IfcProject")
        work_calendar = ifcopenshell.api.sequence.add_work_calendar(self.file)

        # Add work times.
        ifcopenshell.api.sequence.add_work_time(self.file, work_calendar, "WorkingTimes")
        ifcopenshell.api.sequence.add_work_time(self.file, work_calendar, "WorkingTimes")
        ifcopenshell.api.sequence.add_work_time(self.file, work_calendar, "ExceptionTimes")
        ifcopenshell.api.sequence.add_work_time(self.file, work_calendar, "ExceptionTimes")

        # Assign tasks.
        task = ifcopenshell.api.sequence.add_task(self.file)
        ifcopenshell.api.control.assign_control(self.file, work_calendar, [task])

        ifcopenshell.api.sequence.remove_work_calendar(self.file, work_calendar)

        assert len(self.file.by_type("IfcWorkCalendar")) == 0
        assert len(self.file.by_type("IfcWorkTime")) == 0
        assert len(self.file.by_type("IfcTask")) == 1
        assert len(self.file.by_type("IfcRelAssignsToControl")) == 0


class TestRemoveWorkCalendarIFC4X3(test.bootstrap.IFC4X3, TestRemoveWorkCalendar):
    pass
