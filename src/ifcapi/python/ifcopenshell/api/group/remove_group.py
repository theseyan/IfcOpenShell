# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.util.element


def remove_group(file, group):
    for inverse_id in [i.id() for i in file.get_inverse(group)]:
        try:
            inverse = file.by_id(inverse_id)
        except:
            continue
        if inverse.is_a("IfcRelDefinesByProperties"):
            ifcopenshell.api.pset.remove_pset(
                file,
                product=group,
                pset=inverse.RelatingPropertyDefinition,
            )
        elif inverse.is_a("IfcRelAssignsToGroup"):
            if inverse.RelatingGroup == group:
                history = inverse.OwnerHistory
                file.remove(inverse)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)
            elif len(inverse.RelatedObjects) == 1:
                history = inverse.OwnerHistory
                file.remove(inverse)
                if history:
                    ifcopenshell.util.element.remove_deep2(file, history)
    history = group.OwnerHistory
    file.remove(group)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
