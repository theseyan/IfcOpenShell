# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def remove_cost_item_quantity(file, cost_item=None, physical_quantity=None):
    if file.get_total_inverses(physical_quantity) == 1:
        file.remove(physical_quantity)
        return
    quantities = list(cost_item.CostQuantities or [])
    quantities.remove(physical_quantity)
    cost_item.CostQuantities = quantities
