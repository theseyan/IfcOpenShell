# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def add_structural_load_group(
    file,
    name="Unnamed",
    action_type="NOTDEFINED",
    action_source="NOTDEFINED",
):
    load_group = ifcopenshell.api.root.create_entity(
        file,
        ifc_class="IfcStructuralLoadGroup",
        predefined_type="LOAD_GROUP",
        name=name,
    )
    load_group.ActionType = action_type
    load_group.ActionSource = action_source
    return load_group
