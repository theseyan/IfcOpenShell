# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.api.pset


def add_application(
    file,
    application_developer=None,
    version=None,
    application_full_name="IfcOpenShell",
    application_identifier="IfcOpenShell",
):
    if version is None:
        version = ifcopenshell.version
    if not application_developer:
        application_developer = _create_application_organisation(file)
    return file.create_entity(
        "IfcApplication",
        ApplicationDeveloper=application_developer,
        Version=version,
        ApplicationFullName=application_full_name,
        ApplicationIdentifier=application_identifier,
    )


def _create_application_organisation(file):
    result = file.create_entity(
        "IfcOrganization",
        Name="IfcOpenShell",
        Description="IfcOpenShell is an open source software library that helps users and software developers to work with IFC data.",
        Roles=[
            file.create_entity("IfcActorRole", Role="USERDEFINED", UserDefinedRole="CONTRIBUTOR")
        ],
    )
    # 0 IfcOrganization.Identification / Id (IFC2X3).
    result[0] = "IfcOpenShell"

    # 4 IfcOrganization.Addresses
    if file.schema == "IFC4X3":
        # IfcTelecomAddress is deprecated in IFC4X3.
        actor = ifcopenshell.api.owner.add_actor(file, actor=result)
        pset = ifcopenshell.api.pset.add_pset(file, actor, "PEnum_AddressType")
        ifcopenshell.api.pset.edit_pset(
            file,
            pset,
            properties={
                "Purpose": "OTHER",
                "UserDefinedPurpose": "WEBPAGE",
                "WWWHomePageURL": "https://ifcopenshell.org",
            },
        )
    else:
        result[4] = [
            file.create_entity(
                "IfcTelecomAddress",
                Purpose="USERDEFINED",
                UserDefinedPurpose="WEBPAGE",
                WWWHomePageURL="https://ifcopenshell.org",
            ),
        ]
    return result
