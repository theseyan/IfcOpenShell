# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""profile.edit_profile — edit attributes of an IfcProfileDef."""


def edit_profile(file, profile, attributes):
    for name, value in attributes.items():
        setattr(profile, name, value)
