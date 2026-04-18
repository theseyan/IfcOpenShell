# SPDX-License-Identifier: LGPL-3.0-or-later

"""Remove a product (rooted element) from the IFC file."""

import ifcopenshell
import ifcopenshell.util.element


def remove_product(file, product=None):
    """Remove a product and clean up its relationships."""
    if product is None:
        return
    ifcopenshell.util.element.remove_deep2(file, product)
