# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from datetime import datetime
from typing import Union

import ifcopenshell.util.date


def add_date_time(file: ifcopenshell.file, dt: datetime) -> Union[str, ifcopenshell.entity_instance]:
    """Add a new date time.

    Depending on ``file``'s schema method will:
    - IFC2X3 - create IfcDateAndTime entity
    - IFC4+  - create IfcDatetime formatted string

    :param dt: datetime to convert to IFC.
    :return: IfcDateAndTime entity or IfcDatetime string.

    Example:

    .. code:: python

        dt = datetime(2025, 3, 1, 12, 31, 24)
        datetime_ifc = ifcopenshell.api.sequence.add_date_time(self.file, dt)

        # IFC2X3: #1=IfcDateAndTime(#2,#3)
        # IFC4+: "2025-03-01T12:31:24"
        print(datetime_ifc)

    """

    if file.schema == "IFC2X3":
        ifc_dt = file.create_entity("IfcDateAndTime")
        calendar_date_data = ifcopenshell.util.date.datetime2ifc(dt, "IfcCalendarDate")
        assert isinstance(calendar_date_data, dict)
        ifc_dt.DateComponent = file.create_entity("IfcCalendarDate", **calendar_date_data)
        local_time_data = ifcopenshell.util.date.datetime2ifc(dt, "IfcLocalTime")
        assert isinstance(local_time_data, dict)
        ifc_dt.TimeComponent = file.create_entity("IfcLocalTime", **local_time_data)
        return ifc_dt

    dt_str = ifcopenshell.util.date.datetime2ifc(dt, "IfcDateTime")
    assert isinstance(dt_str, str)
    return dt_str
