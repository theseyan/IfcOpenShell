# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""layer.add_layer_with_style — create a new presentation layer with style."""

import ifcopenshell


def add_layer_with_style(file, name="Unnamed", on="UNKNOWN", frozen="UNKNOWN", blocked="UNKNOWN", styles=()):
    return file.create_entity(
        "IfcPresentationLayerWithStyle",
        Name=name,
        LayerOn=on,
        LayerFrozen=frozen,
        LayerBlocked=blocked,
        LayerStyles=styles,
    )
