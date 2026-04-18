# SPDX-License-Identifier: LGPL-3.0-or-later

"""layer.assign_layer — assign representation items to a layer."""

import ifcopenshell


def assign_layer(file, items, layer):
    assigned_items = set(layer.AssignedItems or [])
    items_set = set(items)
    if items_set.issubset(assigned_items):
        return
    layer.AssignedItems = list(assigned_items | items_set)
