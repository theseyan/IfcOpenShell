# SPDX-License-Identifier: LGPL-3.0-or-later

"""layer.add_layer — create a new presentation layer assignment."""

import ifcopenshell


def add_layer(file, name="Unnamed"):
    return file.create_entity("IfcPresentationLayerAssignment", Name=name)
