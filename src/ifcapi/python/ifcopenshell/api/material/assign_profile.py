# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.representation


def assign_profile(file, material_profile, profile):
    # TODO: handle composite profiles
    old_profile = material_profile.Profile
    material_profile.Profile = profile
    for profile_set in material_profile.ToMaterialProfileSet:
        for inverse in file.get_inverse(profile_set):
            if not inverse.is_a("IfcMaterialProfileSetUsage"):
                continue
            if file.schema == "IFC2X3":
                for rel in file.get_inverse(inverse):
                    if not rel.is_a("IfcRelAssociatesMaterial"):
                        continue
                    for element in rel.RelatedObjects:
                        _change_profile(file, element, profile)
            else:
                for rel in inverse.AssociatedTo:
                    for element in rel.RelatedObjects:
                        _change_profile(file, element, profile)

    if old_profile and file.get_total_inverses(old_profile) == 0:
        # TODO: check remove deep
        file.remove(old_profile)


def _change_profile(file, element, profile):
    representation = ifcopenshell.util.representation.get_representation(element, "Model", "Body", "MODEL_VIEW")
    if not representation:
        return
    for subelement in file.traverse(representation):
        if subelement.is_a("IfcSweptAreaSolid"):
            subelement.SweptArea = profile
