# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_role(file, assigned_object=None, role="ARCHITECT"):
    element = file.create_entity("IfcActorRole", Role="ARCHITECT")
    if role:
        try:
            element.Role = role
        except:
            element.Role = "USERDEFINED"
            element.UserDefinedRole = role
    roles = list(assigned_object.Roles) if assigned_object.Roles else []
    roles.append(element)
    assigned_object.Roles = roles
    return element
