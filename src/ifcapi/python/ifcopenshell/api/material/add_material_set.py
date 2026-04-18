# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def add_material_set(file, name="Unnamed", set_type="IfcMaterialConstituentSet"):
    if set_type == "IfcMaterialLayerSet":
        return file.create_entity("IfcMaterialLayerSet", LayerSetName=name or "Unnamed")
    elif set_type == "IfcMaterialList":
        return file.create_entity("IfcMaterialList")
    return file.create_entity(set_type, Name=name or "Unnamed")
