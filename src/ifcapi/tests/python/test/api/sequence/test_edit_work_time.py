# SPDX-License-Identifier: LGPL-3.0-or-later

import datetime

import ifcopenshell.api.sequence
import test.bootstrap


# NOTE: IfcWorkTime was introduced in IFC4
class TestEditWorkTime(test.bootstrap.IFC4):
    def test_run(self):
        work_time = self.file.createIfcWorkTime()
        recurrence_pattern = self.file.createIfcRecurrencePattern()
        attributes = {
            "Name": "Test",
            "DataOrigin": "USERDEFINED",
            "UserDefinedDataOrigin": "Custom",
            "RecurrencePattern": recurrence_pattern,
            "Start": datetime.datetime(2020, 1, 1),
            "Finish": datetime.datetime(2020, 2, 1),
        }
        ifcopenshell.api.sequence.edit_work_time(self.file, work_time=work_time, attributes=attributes)
        assert work_time.Name == attributes["Name"]
        assert work_time.DataOrigin == attributes["DataOrigin"]
        assert work_time.UserDefinedDataOrigin == attributes["UserDefinedDataOrigin"]
        assert work_time.RecurrencePattern == attributes["RecurrencePattern"]
        assert work_time[4] == "2020-01-01"
        assert work_time[5] == "2020-02-01"


class TestEditWorkTimeIFC4X3(test.bootstrap.IFC4X3):
    def test_run(self):
        work_time = self.file.createIfcWorkTime()
        recurrence_pattern = self.file.createIfcRecurrencePattern()
        attributes = {
            "Name": "Test",
            "DataOrigin": "USERDEFINED",
            "UserDefinedDataOrigin": "Custom",
            "RecurrencePattern": recurrence_pattern,
            "StartDate": datetime.datetime(2020, 1, 1),
            "FinishDate": datetime.datetime(2020, 2, 1),
        }
        ifcopenshell.api.sequence.edit_work_time(self.file, work_time=work_time, attributes=attributes)
        assert work_time.Name == attributes["Name"]
        assert work_time.DataOrigin == attributes["DataOrigin"]
        assert work_time.UserDefinedDataOrigin == attributes["UserDefinedDataOrigin"]
        assert work_time.RecurrencePattern == attributes["RecurrencePattern"]
        assert work_time.StartDate == "2020-01-01"
        assert work_time.FinishDate == "2020-02-01"

    def test_ifc4_code_to_work_in_ifc4x3(self):
        TestEditWorkTime.test_run(self)
