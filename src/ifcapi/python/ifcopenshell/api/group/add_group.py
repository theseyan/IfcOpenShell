# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def add_group(file, name="Unnamed", description=None):
    return file.create_entity(
        "IfcGroup",
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        Name=name,
        Description=description,
    )
