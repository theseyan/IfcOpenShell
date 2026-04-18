# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_group(file, group, attributes):
    for name, value in attributes.items():
        setattr(group, name, value)
