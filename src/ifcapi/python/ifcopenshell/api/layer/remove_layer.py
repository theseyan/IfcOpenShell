# SPDX-License-Identifier: LGPL-3.0-or-later

"""layer.remove_layer — remove a presentation layer assignment."""

import ifcopenshell


def remove_layer(file, layer):
    file.remove(layer)
