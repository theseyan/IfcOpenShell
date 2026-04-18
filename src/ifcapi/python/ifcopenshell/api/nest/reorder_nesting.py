# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def reorder_nesting(file, item, old_index=0, new_index=0):
    if not item.Nests:
        return
    nesting_set = item.Nests[0]
    if not old_index:
        old_index = nesting_set.RelatedObjects.index(item)
    items = list(getattr(nesting_set, "RelatedObjects") or [])
    items.insert(new_index, items.pop(old_index))
    setattr(nesting_set, "RelatedObjects", items)
