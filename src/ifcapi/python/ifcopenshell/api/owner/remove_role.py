# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def remove_role(file, role=None):
    for inverse in file.get_inverse(role):
        if inverse.is_a() in ("IfcOrganization", "IfcPerson", "IfcPersonAndOrganization"):
            if inverse.Roles == (role,):
                inverse.Roles = None
        elif inverse.is_a("IfcResourceLevelRelationship"):
            if inverse.RelatedResourceObjects == (role,):
                file.remove(inverse)
    file.remove(role)
