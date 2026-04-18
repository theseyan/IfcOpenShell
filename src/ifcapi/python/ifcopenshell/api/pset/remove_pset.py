# SPDX-License-Identifier: LGPL-3.0-or-later

"""Remove a property set from a product."""

import ifcopenshell
import ifcopenshell.util.element


def remove_pset(file, product, pset):
    """Remove *pset* from *product*, cleaning up orphaned entities."""
    to_purge = []
    should_remove_pset = True

    for inverse in file.get_inverse(pset):
        if inverse.is_a("IfcRelDefinesByProperties"):
            if not inverse.RelatedObjects or len(inverse.RelatedObjects) == 1:
                to_purge.append(inverse)
            else:
                related_objects = list(inverse.RelatedObjects)
                related_objects.remove(product)
                inverse.RelatedObjects = related_objects
                should_remove_pset = False

    if should_remove_pset:
        properties = []
        if pset.is_a("IfcPropertySet"):
            properties = pset.HasProperties or ()
        elif pset.is_a("IfcQuantitySet"):
            properties = getattr(pset, "Quantities", ()) or ()
        elif pset.is_a() in ("IfcMaterialProperties", "IfcProfileProperties"):
            properties = getattr(pset, "Properties", ()) or ()

        for prop in properties:
            if file.get_total_inverses(prop) != 1:
                continue
            if prop.is_a("IfcPropertyEnumeratedValue"):
                enumeration = getattr(prop, "EnumerationReference", None)
                if enumeration and file.get_total_inverses(enumeration) == 1:
                    file.remove(enumeration)
            file.remove(prop)

        history = getattr(pset, "OwnerHistory", None)
        file.remove(pset)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)

    for element in to_purge:
        history = getattr(element, "OwnerHistory", None)
        file.remove(element)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
