# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_actor(file, actor=None):
    history = actor.OwnerHistory
    file.remove(actor)
    if history:
        ifcopenshell.util.element.remove_deep2(file, history)
