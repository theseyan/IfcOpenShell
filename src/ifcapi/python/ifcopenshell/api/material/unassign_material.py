# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""material.unassign_material — remove material assignment from products."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_material(file, products):
    if not products:
        return
    products_set = set(products)

    # Remove material usages from types
    for product in products_set:
        if not product.is_a("IfcTypeObject"):
            continue
        material = ifcopenshell.util.element.get_material(product)
        if not material:
            continue
        if material.is_a() in ("IfcMaterialLayerSet", "IfcMaterialProfileSet"):
            for inverse in file.get_inverse(material):
                if file.schema == "IFC2X3":
                    if not inverse.is_a("IfcMaterialLayerSetUsage"):
                        continue
                    for inverse2 in file.get_inverse(inverse):
                        if inverse2.is_a("IfcRelAssociatesMaterial"):
                            history = inverse2.OwnerHistory
                            file.remove(inverse2)
                            if history:
                                ifcopenshell.util.element.remove_deep2(file, history)
                else:
                    if not inverse.is_a("IfcMaterialUsageDefinition"):
                        continue
                    for rel in getattr(inverse, "AssociatedTo", ()) or ():
                        history = rel.OwnerHistory
                        file.remove(rel)
                        if history:
                            ifcopenshell.util.element.remove_deep2(file, history)
                file.remove(inverse)

    # Unassign materials from products
    associations = set()
    for product in products_set:
        for assoc in getattr(product, "HasAssociations", ()) or ():
            associations.add(assoc)

    while associations:
        rel = next(iter(associations))
        if not rel.is_a("IfcRelAssociatesMaterial"):
            associations.remove(rel)
            continue

        material = rel.RelatingMaterial
        related_objects = set(rel.RelatedObjects) - products_set

        if material.is_a() in ("IfcMaterialLayerSetUsage", "IfcMaterialProfileSetUsage"):
            material_inverses = set(file.get_inverse(material))
            if material_inverses.issubset(associations) and not related_objects:
                file.remove(material)
        associations.remove(rel)

        if not related_objects:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
            continue
        rel.RelatedObjects = list(related_objects)
        ifcopenshell.api.owner.update_owner_history(file, element=rel)
