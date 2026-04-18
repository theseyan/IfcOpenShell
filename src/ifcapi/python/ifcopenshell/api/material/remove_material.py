# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""material.remove_material — remove an IfcMaterial from the file."""

import ifcopenshell
import ifcopenshell.util.element


def remove_material(file, material):
    inverse_elements = file.get_inverse(material)
    file.remove(material)
    for inverse in inverse_elements:
        if inverse.is_a("IfcMaterialConstituent"):
            file.remove(inverse)
        elif inverse.is_a("IfcMaterialLayer"):
            file.remove(inverse)
        elif inverse.is_a("IfcMaterialProfile"):
            file.remove(inverse)
        elif inverse.is_a("IfcRelAssociatesMaterial"):
            history = inverse.OwnerHistory
            file.remove(inverse)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
        elif inverse.is_a("IfcMaterialProperties"):
            if file.schema != "IFC2X3":
                props = inverse.Properties
            else:
                props = getattr(inverse, "ExtendedProperties", None)
            for prop in (props or []):
                file.remove(prop)
            file.remove(inverse)
        elif inverse.is_a("IfcMaterialDefinitionRepresentation"):
            for representation in inverse.Representations:
                for item in representation.Items:
                    file.remove(item)
                file.remove(representation)
            file.remove(inverse)
