# SPDX-License-Identifier: LGPL-3.0-or-later

"""profile.remove_profile — remove an IfcProfileDef."""

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.util.element


def remove_profile(file, profile):
    is_ifc2x3 = file.schema == "IFC2X3"

    subelements = set()
    for attribute in profile:
        if isinstance(attribute, ifcopenshell.entity_instance):
            subelements.add(attribute)

    profile_psets = []
    if is_ifc2x3:
        for pset in file.by_type("IfcProfileProperties"):
            if pset.ProfileDefinition != profile:
                continue
            profile_psets.append(pset)
    else:
        profile_psets = profile.HasProperties

    for pset in profile_psets:
        ifcopenshell.api.pset.remove_pset(file, product=profile, pset=pset)

    file.remove(profile)
    for subelement in subelements:
        ifcopenshell.util.element.remove_deep2(file, subelement)
