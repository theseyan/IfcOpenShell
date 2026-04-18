# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.unit


def add_layer(file, layer_set, material, name=None):
    unit_scale = ifcopenshell.util.unit.calculate_unit_scale(file)
    layers = list(layer_set.MaterialLayers or [])
    if file.schema == "IFC2X3":
        layer = file.create_entity("IfcMaterialLayer", Material=material, LayerThickness=0.1 / unit_scale)
    else:
        layer = file.create_entity("IfcMaterialLayer", Material=material, LayerThickness=0.1 / unit_scale, Name=name)
    layers.append(layer)
    layer_set.MaterialLayers = layers
    return layer
