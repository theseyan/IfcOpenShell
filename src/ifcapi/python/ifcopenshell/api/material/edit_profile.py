# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_profile(file, profile, attributes=None, profile_def=None, material=None):
    for name, value in (attributes or {}).items():
        setattr(profile, name, value)
    if material:
        profile.Material = material
    if profile_def:
        profile.Profile = profile_def
