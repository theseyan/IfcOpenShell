# SPDX-License-Identifier: LGPL-3.0-or-later

import time

import ifcopenshell
import ifcopenshell.api.owner.settings


def create_owner_history(file):
    user = ifcopenshell.api.owner.settings.get_user(file)
    if file.schema != "IFC2X3" and not user:
        return
    application = ifcopenshell.api.owner.settings.get_application(file)
    if file.schema != "IFC2X3" and not application:
        return
    return file.create_entity(
        "IfcOwnerHistory",
        OwningUser=user,
        OwningApplication=application,
        State="READWRITE",
        ChangeAction="ADDED",
        LastModifiedDate=int(time.time()),
        LastModifyingUser=user,
        LastModifyingApplication=application,
        CreationDate=int(time.time()),
    )
