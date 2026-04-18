# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new property set to a product."""

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.api.pset
import ifcopenshell.guid


def add_pset(file, product, name, ifc2x3_subclass=None):
    """Add a new property set to *product* and return it.

    If a property set with the same *name* is already assigned, it is returned
    without creating a duplicate.
    """
    is_ifc2x3 = file.schema == "IFC2X3"

    if product.is_a("IfcObject") or product.is_a("IfcContext"):
        for rel in getattr(product, "IsDefinedBy", ()) or ():
            if rel.is_a("IfcRelDefinesByProperties") and rel.RelatingPropertyDefinition.Name == name:
                return rel.RelatingPropertyDefinition

        pset = file.create_entity(
            "IfcPropertySet",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            Name=name,
        )
        ifcopenshell.api.pset.assign_pset(file, [product], pset)
        return pset

    elif product.is_a("IfcTypeObject"):
        for definition in getattr(product, "HasPropertySets", ()) or ():
            if definition.Name == name:
                return definition

        pset = file.create_entity(
            "IfcPropertySet",
            GlobalId=ifcopenshell.guid.new(),
            OwnerHistory=ifcopenshell.api.owner.create_owner_history(file),
            Name=name,
        )
        ifcopenshell.api.pset.assign_pset(file, [product], pset)
        return pset

    elif product.is_a("IfcMaterialDefinition") or product.is_a("IfcMaterial"):
        kwargs = {"Material": product}
        if is_ifc2x3:
            ifc_class = ifc2x3_subclass or "IfcExtendedMaterialProperties"
            definitions = (d for d in file.by_type("IfcMaterialProperties") if d.Material == product)
            if ifc_class == "IfcExtendedMaterialProperties":
                kwargs["Name"] = name
        else:
            ifc_class = "IfcMaterialProperties"
            definitions = getattr(product, "HasProperties", ()) or ()
            kwargs["Name"] = name
        for definition in definitions:
            if getattr(definition, "Name", None) == name:
                return definition
        return file.create_entity(ifc_class, **kwargs)

    elif product.is_a("IfcProfileDef"):
        if not is_ifc2x3:
            for definition in getattr(product, "HasProperties", ()) or ():
                if definition.Name == name:
                    return definition

        kwargs = {"ProfileDefinition": product}
        if not is_ifc2x3:
            kwargs["Name"] = name

        if is_ifc2x3:
            ifc_class = ifc2x3_subclass or "IfcGeneralProfileProperties"
        else:
            ifc_class = "IfcProfileProperties"
        return file.create_entity(ifc_class, **kwargs)

    raise TypeError(f"Class '{product.is_a()}' doesn't support adding a property set.")
