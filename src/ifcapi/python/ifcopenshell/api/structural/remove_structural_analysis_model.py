# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_structural_analysis_model(file, structural_analysis_model=None):
    for rel in structural_analysis_model.IsGroupedBy or []:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
    history = structural_analysis_model.OwnerHistory
    file.remove(structural_analysis_model)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
