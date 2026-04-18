# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_person(file, identification="HSeldon", family_name="Seldon", given_name="Hari"):
    data = {"FamilyName": family_name, "GivenName": given_name}
    if file.schema == "IFC2X3":
        data["Id"] = identification
    else:
        data["Identification"] = identification
    return file.create_entity("IfcPerson", **data)
