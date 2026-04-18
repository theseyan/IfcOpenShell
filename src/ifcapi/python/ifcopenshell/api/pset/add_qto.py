# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def add_qto(file, product, name):
    if product.is_a("IfcObject") or product.is_a("IfcContext"):
        for rel in product.IsDefinedBy or []:
            if (
                rel.is_a("IfcRelDefinesByProperties")
                and rel.RelatingPropertyDefinition.Name == name
            ):
                return rel.RelatingPropertyDefinition

        qto = _create_qto(file, name)
        file.create_entity(
            "IfcRelDefinesByProperties",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedObjects": [product],
                "RelatingPropertyDefinition": qto,
            }
        )
        return qto
    elif product.is_a("IfcTypeObject"):
        for definition in product.HasPropertySets or []:
            if definition.Name == name:
                return definition
        qto = _create_qto(file, name)
        has_property_sets = list(product.HasPropertySets or [])
        has_property_sets.append(qto)
        product.HasPropertySets = has_property_sets
        return qto


def _create_qto(file, name):
    return file.create_entity(
        "IfcElementQuantity",
        GlobalId=ifcopenshell.guid.new(),
        OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
        Name=name,
        MethodOfMeasurement="BaseQuantities" if name.endswith("BaseQuantities") else None,
    )
