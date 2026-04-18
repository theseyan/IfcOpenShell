# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import ifcopenshell.util.element


def remove_structural_connection_condition(file, relation=None):
    if relation.AppliedCondition:
        ifcopenshell.api.structural.remove_structural_boundary_condition(
            file,
            connection=relation.RelatedStructuralConnection,
        )
    history = relation.OwnerHistory
    file.remove(relation)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
