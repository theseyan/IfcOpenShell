# SPDX-License-Identifier: LGPL-3.0-or-later

"""profile.copy_profile — copy an IfcProfileDef."""

import ifcopenshell.util.element


def copy_profile(file, profile):
    new_profile = ifcopenshell.util.element.copy_deep(file, profile)
    inverses = file.get_inverse(profile)
    psets = [i for i in inverses if i.is_a("IfcProfileProperties")]
    for pset in psets:
        new_pset = ifcopenshell.util.element.copy(file, pset)
        new_pset.ProfileDefinition = new_profile
    return new_profile
