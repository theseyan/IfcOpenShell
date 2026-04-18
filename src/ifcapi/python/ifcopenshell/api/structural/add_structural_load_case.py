# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def add_structural_load_case(
    file,
    name="Unnamed",
    action_type="NOTDEFINED",
    action_source="NOTDEFINED",
):
    load_case = ifcopenshell.api.root.create_entity(
        file, ifc_class="IfcStructuralLoadCase", predefined_type="LOAD_CASE", name=name
    )
    load_case.ActionType = action_type
    load_case.ActionSource = action_source
    return load_case
