# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_cost_item_quantity(file, physical_quantity=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(physical_quantity, name, value)
