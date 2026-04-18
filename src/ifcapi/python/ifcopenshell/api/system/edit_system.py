# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_system(file, system, attributes):
    for name, value in attributes.items():
        setattr(system, name, value)
