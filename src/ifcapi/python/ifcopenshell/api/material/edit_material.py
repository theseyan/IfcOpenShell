# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_material(file, material, attributes):
    for name, value in attributes.items():
        setattr(material, name, value)
