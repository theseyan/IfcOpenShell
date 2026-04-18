# SPDX-License-Identifier: LGPL-3.0-or-later

from datetime import datetime

import ifcopenshell.api.sequence
import ifcopenshell.util
import ifcopenshell.util.date
import test.bootstrap


class TestAddDateTime(test.bootstrap.IFC4):
    def test_run(self):
        dt = datetime(2025, 3, 1, 12, 31, 24)
        res = ifcopenshell.api.sequence.add_date_time(self.file, dt)
        if self.file.schema == "IFC2X3":
            assert isinstance(res, ifcopenshell.entity_instance)
            assert res.is_a("IfcDateAndTime")
            assert ifcopenshell.util.date.ifc2datetime(res) == dt
            print(res)
        else:
            assert res == "2025-03-01T12:31:24"


class TestAddDateTimeIFC2X3(test.bootstrap.IFC2X3, TestAddDateTime):
    pass


class TestAddDateTimeIFC4X3(test.bootstrap.IFC4X3, TestAddDateTime):
    pass
