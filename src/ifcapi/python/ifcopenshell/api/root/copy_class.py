# IfcOpenShell - IFC toolkit and geometry engine
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi


def copy_class(
    file: ifcopenshell.file, product: ifcopenshell.entity_instance
) -> ifcopenshell.entity_instance:
    """Copies a product

    The following relationships are also duplicated:

    * The copy will have the same object placement coordinates as the original.
    * The copy will have duplicated property sets, properties, and quantities.
    * The copy will have all nested distribution ports copied too.
    * The copy will be part of the same aggregate and spatial container.
    * The copy, if it is an occurrence, will have the same type.
    * Unfilled voids, material usages and sets, and group memberships are copied.

    Representations, filled voids, and path connectivity are not copied.

    :param product: The IfcProduct to copy.
    :return: The copied product.
    """
    handle = _capi.root_copy_class(file._handle, product._handle)
    return ifcopenshell.entity_instance(file, handle)
