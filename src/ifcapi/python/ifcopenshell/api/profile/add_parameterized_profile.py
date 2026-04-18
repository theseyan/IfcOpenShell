# SPDX-License-Identifier: LGPL-3.0-or-later

"""profile.add_parameterized_profile — create an IfcParameterizedProfileDef."""

import ifcopenshell


def add_parameterized_profile(file, ifc_class, profile_type="AREA"):
    return file.create_entity(ifc_class, ProfileType=profile_type)
