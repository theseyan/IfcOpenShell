# SPDX-License-Identifier: LGPL-3.0-or-later

import time

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.api.owner.settings
import ifcopenshell.util.element


def update_owner_history(file, element=None):
    if not element.is_a("IfcRoot"):
        return
    user = ifcopenshell.api.owner.settings.get_user(file)
    if not user:
        return
    application = ifcopenshell.api.owner.settings.get_application(file)
    if not application:
        return

    # 1 IfcRoot IfcOwnerHistory
    owner_history = element[1]
    if not owner_history:
        owner_history = ifcopenshell.api.owner.create_owner_history(file)
        element[1] = owner_history
        return owner_history

    if file.get_total_inverses(owner_history) > 1:
        owner_history = ifcopenshell.util.element.copy(file, owner_history)
        element[1] = owner_history

    # 3 IfcOwnerHistory ChangeAction
    owner_history[3] = "MODIFIED"
    # 4 IfcOwnerHistory LastModifiedDate
    owner_history[4] = int(time.time())
    # 5 IfcOwnerHistory LastModifyingUser
    owner_history[5] = user
    # 6 IfcOwnerHistory LastModifyingApplication
    owner_history[6] = application
    return owner_history
