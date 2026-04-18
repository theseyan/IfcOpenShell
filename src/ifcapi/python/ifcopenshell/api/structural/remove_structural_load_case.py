# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_structural_load_case(file, load_case=None):
    for rel in load_case.IsGroupedBy or []:
        history = rel.OwnerHistory
        file.remove(rel)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
    history = load_case.OwnerHistory
    file.remove(load_case)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
