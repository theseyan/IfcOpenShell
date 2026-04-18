# SPDX-License-Identifier: LGPL-3.0-or-later

"""system.assign_system — assign distribution elements to a system."""

import ifcopenshell
import ifcopenshell.api.group
import ifcopenshell.util.system


def assign_system(file, products, system):
    for product in products:
        if not ifcopenshell.util.system.is_assignable(product, system):
            raise TypeError(f"You cannot assign an {product.is_a()} to an {system.is_a()}")
    return ifcopenshell.api.group.assign_group(file, products=products, group=system)
