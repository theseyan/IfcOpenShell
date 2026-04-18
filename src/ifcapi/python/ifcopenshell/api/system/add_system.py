# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""system.add_system — create a new distribution system."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def add_system(file, ifc_class="IfcDistributionSystem"):
    if file.schema == "IFC2X3" and ifc_class == "IfcDistributionSystem":
        ifc_class = "IfcSystem"
    return file.create_entity(
        ifc_class,
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        Name="Unnamed",
    )
