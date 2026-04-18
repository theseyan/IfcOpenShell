# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""material.assign_material — assign a material or material set to products."""

from collections import defaultdict

import ifcopenshell
import ifcopenshell.api.material
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def assign_material(file, products, type="IfcMaterial", material=None):
    products_set = set(products)
    if not products_set:
        return None

    to_unassign = [p for p in products_set if ifcopenshell.util.element.get_material(p)]
    if to_unassign:
        ifcopenshell.api.material.unassign_material(file, products=to_unassign)

    if type == "IfcMaterial" or (
        material and not material.is_a("IfcMaterial") and not type.endswith("Usage")
    ):
        return _assign_ifc_material(file, products_set, material)

    if type == "IfcMaterialConstituentSet":
        material_set = file.create_entity(type)
        return _create_material_association(file, products_set, material_set)

    if type == "IfcMaterialLayerSet":
        material_set = file.create_entity(type)
        return _create_material_association(file, products_set, material_set)

    if type == "IfcMaterialLayerSetUsage":
        return _assign_layer_set_usage(file, products_set, material)

    if type == "IfcMaterialProfileSet":
        material_set = file.create_entity(type)
        return _create_material_association(file, products_set, material_set)

    if type == "IfcMaterialProfileSetUsage":
        return _assign_profile_set_usage(file, products_set, material)

    if type == "IfcMaterialList":
        material_set = file.create_entity(type)
        material_set.Materials = [material]
        return _create_material_association(file, products_set, material_set)


def _assign_ifc_material(file, products_set, material):
    material = material or file.create_entity("IfcMaterial")
    rel = _get_rel_associates_material(file, material)
    if not rel:
        return _create_material_association(file, products_set, material)
    previous = set(rel.RelatedObjects)
    rel.RelatedObjects = list(previous | products_set)
    ifcopenshell.api.owner.update_owner_history(file, element=rel)
    return rel


def _assign_layer_set_usage(file, products_set, provided_material):
    AXIS3_CLASSES = [
        "IfcSlab", "IfcSlabStandardCase", "IfcSlabElementedCase",
        "IfcRoof", "IfcRamp", "IfcPlate", "IfcPlateStandardCase",
        "IfcCovering", "IfcFurniture",
    ]

    layer_types_to_products = defaultdict(list)

    for product in products_set:
        if provided_material is not None:
            material_set = provided_material
        else:
            element_type = ifcopenshell.util.element.get_type(product)
            element_type_material = None
            if element_type is not None:
                element_type_material = ifcopenshell.util.element.get_material(element_type)
            if element_type_material and element_type_material.is_a("IfcMaterialLayerSet"):
                material_set = element_type_material
            else:
                material_set = file.create_entity("IfcMaterialLayerSet")

        layer_set_direction = "AXIS3" if product.is_a() in AXIS3_CLASSES else "AXIS2"
        layer_types_to_products[(material_set, layer_set_direction)].append(product)

    rels = []
    for (material_set, layer_set_direction), prods in layer_types_to_products.items():
        usage = file.create_entity("IfcMaterialLayerSetUsage", **{
            "ForLayerSet": material_set,
            "LayerSetDirection": layer_set_direction,
            "DirectionSense": "POSITIVE",
            "OffsetFromReferenceLine": 0,
        })
        rels.append(_create_material_association(file, prods, usage))
    return rels[0] if len(rels) == 1 else rels


def _assign_profile_set_usage(file, products_set, provided_material):
    material_sets_to_products = defaultdict(list)

    for product in products_set:
        if provided_material is not None:
            material_set = provided_material
        else:
            element_type = ifcopenshell.util.element.get_type(product)
            element_type_material = None
            if element_type is not None:
                element_type_material = ifcopenshell.util.element.get_material(element_type)
            if element_type_material and element_type_material.is_a("IfcMaterialProfileSet"):
                material_set = element_type_material
            else:
                material_set = file.create_entity("IfcMaterialProfileSet")

        material_sets_to_products[material_set].append(product)

    rels = []
    for material_set, prods in material_sets_to_products.items():
        usage = file.create_entity("IfcMaterialProfileSetUsage", **{"ForProfileSet": material_set})
        rels.append(_create_material_association(file, prods, usage))
    return rels[0] if len(rels) == 1 else rels


def _create_material_association(file, products, relating_material):
    if not isinstance(products, list):
        products = list(products)
    return file.create_entity("IfcRelAssociatesMaterial", **{
        "GlobalId": ifcopenshell.guid.new(),
        "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
        "RelatedObjects": products,
        "RelatingMaterial": relating_material,
    })


def _get_rel_associates_material(file, material):
    if file.schema == "IFC2X3" or material.is_a("IfcMaterialList"):
        for r in file.by_type("IfcRelAssociatesMaterial"):
            if r.RelatingMaterial == material:
                return r
        return None
    return next(iter(getattr(material, "AssociatedTo", ()) or ()), None)
