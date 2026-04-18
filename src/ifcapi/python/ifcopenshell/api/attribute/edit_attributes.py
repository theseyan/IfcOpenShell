# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""attribute.edit_attributes — set attributes on an IFC product."""

import ifcopenshell.api.owner
import ifcopenshell.util.element


def edit_attributes(file, product, attributes):
    for name, value in attributes.items():
        setattr(product, name, value)

    predefined_type = getattr(product, "PredefinedType", ...)
    if predefined_type is not ...:
        element_type = getattr(product, "ElementType", ...)
        if element_type is not ...:
            if element_type is None and predefined_type == "USERDEFINED":
                product.PredefinedType = "NOTDEFINED"
            elif element_type and predefined_type != "USERDEFINED":
                product.PredefinedType = "USERDEFINED"
        else:
            object_type = getattr(product, "ObjectType", ...)
            if object_type is not ...:
                relating_type = ifcopenshell.util.element.get_type(product)
                if relating_type and getattr(relating_type, "PredefinedType", None) not in ("NOTDEFINED", None):
                    product.ObjectType = None
                    product.PredefinedType = None
                elif object_type is None and predefined_type == "USERDEFINED":
                    product.PredefinedType = "NOTDEFINED"
                elif object_type and predefined_type != "USERDEFINED":
                    product.PredefinedType = "USERDEFINED"

    if hasattr(product, "OwnerHistory"):
        ifcopenshell.api.owner.update_owner_history(file, element=product)
