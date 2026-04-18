# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_organisation(file, identification="APTR", name="Aperture Science"):
    data = {"Name": name}
    if file.schema == "IFC2X3":
        data["Id"] = identification
    else:
        data["Identification"] = identification
    return file.create_entity("IfcOrganization", **data)
