# SPDX-License-Identifier: LGPL-3.0-or-later

"""system.remove_system — remove a system, keeping its elements."""

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.util.element


def remove_system(file, system):
    for inverse_id in [i.id() for i in file.get_inverse(system)]:
        try:
            inverse = file.by_id(inverse_id)
        except Exception:
            continue
        if inverse.is_a("IfcRelDefinesByProperties"):
            ifcopenshell.api.pset.remove_pset(
                file, product=system, pset=inverse.RelatingPropertyDefinition,
            )
        elif inverse.is_a("IfcRelAssignsToGroup"):
            if inverse.RelatingGroup == system:
                history = inverse.OwnerHistory
                file.remove(inverse)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)
            elif len(inverse.RelatedObjects) == 1:
                history = inverse.OwnerHistory
                file.remove(inverse)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)
    history = system.OwnerHistory
    file.remove(system)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
