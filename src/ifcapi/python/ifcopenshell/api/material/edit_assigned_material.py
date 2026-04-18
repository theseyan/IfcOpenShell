# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_assigned_material(file, element, attributes):
    for name, value in attributes.items():
        setattr(element, name, value)
