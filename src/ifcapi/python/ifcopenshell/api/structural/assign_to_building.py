# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_to_building(file, structural_analysis_model=None, building=None):
    for rel in structural_analysis_model.ServicesBuildings or []:
        if building in rel.RelatedBuildings:
            return rel
        rel.RelatedBuildings = list(rel.RelatedBuildings) + [building]
        return rel

    return file.create_entity(
        "IfcRelServicesBuildings",
        ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        RelatingSystem=structural_analysis_model,
        RelatedBuildings=[building],
    )
