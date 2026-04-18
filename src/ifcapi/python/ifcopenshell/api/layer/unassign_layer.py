# SPDX-License-Identifier: LGPL-3.0-or-later

"""layer.unassign_layer — remove representation items from a layer."""

import ifcopenshell


def unassign_layer(file, items, layer):
    assigned_items = set(layer.AssignedItems) or set()
    items_set = set(items)
    if not items_set.issubset(assigned_items):
        return
    assigned_items = list(assigned_items - items_set)
    if assigned_items:
        layer.AssignedItems = assigned_items
    else:
        file.remove(layer)
