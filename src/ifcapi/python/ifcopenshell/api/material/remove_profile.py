# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_profile(file, profile, should_remove_profile_def=False, should_remove_material=False):
    subelements = set()
    for attribute in profile:
        if isinstance(attribute, ifcopenshell.entity_instance):
            subelements.add(attribute)
    file.remove(profile)
    for subelement in subelements:
        if subelement.is_a("IfcMaterial") and not should_remove_material:
            continue
        elif subelement.is_a("IfcProfileDef") and not should_remove_profile_def:
            continue
        ifcopenshell.util.element.remove_deep2(file, subelement)
