# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""layer.edit_layer — edit attributes of an IfcPresentationLayerAssignment."""


def edit_layer(file, layer, attributes):
    for name, value in attributes.items():
        setattr(layer, name, value)
