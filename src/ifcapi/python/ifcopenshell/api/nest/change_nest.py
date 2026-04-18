# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.nest
import ifcopenshell.api.owner
import ifcopenshell.util.element


def change_nest(file, item, new_parent):
    if not item.Nests:
        return
    nests = item.Nests[0]
    related_objects = list(nests.RelatedObjects)
    related_objects.remove(item)
    if related_objects:
        nests.RelatedObjects = related_objects
        ifcopenshell.api.owner.update_owner_history(file, element=nests)
    else:
        history = nests.OwnerHistory
        file.remove(nests)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
    ifcopenshell.api.nest.assign_object(file, related_objects=[item], relating_object=new_parent)
