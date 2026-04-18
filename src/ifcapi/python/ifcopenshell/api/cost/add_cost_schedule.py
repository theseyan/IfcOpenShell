# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from datetime import datetime

import ifcopenshell.api.root


def add_cost_schedule(file, name=None, predefined_type="NOTDEFINED"):
    cost_schedule = ifcopenshell.api.root.create_entity(
        file,
        ifc_class="IfcCostSchedule",
        predefined_type=predefined_type,
        name=name,
    )
    cost_schedule.UpdateDate = datetime.now().isoformat()
    return cost_schedule
