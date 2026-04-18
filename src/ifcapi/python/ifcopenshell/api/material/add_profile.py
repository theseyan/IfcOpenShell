# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_profile(file, profile_set, material=None, profile=None, name=None):
    profiles = list(profile_set.MaterialProfiles or [])
    mat_profile = file.create_entity("IfcMaterialProfile", Name=name)
    if material:
        mat_profile.Material = material
    if profile:
        mat_profile.Profile = profile
    profiles.append(mat_profile)
    profile_set.MaterialProfiles = profiles
    return mat_profile
